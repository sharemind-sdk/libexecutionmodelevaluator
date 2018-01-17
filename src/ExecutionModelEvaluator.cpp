/*
 * Copyright (C) Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#include "ExecutionModelEvaluator.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <exprtk.hpp>
#include <sharemind/Concat.h>
#include <sharemind/MakeUnique.h>


namespace {

class ExprTkModel final: public sharemind::ExecutionModelEvaluator::Model {

public: /* Methods: */

    ExprTkModel(exprtk::parser<double> & parser,
                std::string const & model,
                std::string const & inputSizeVariable,
                std::map<std::string, double> const & constants)
    {
        m_symbolTable.add_variable(inputSizeVariable, m_inputSize);

        for (auto const & pair : constants)
            m_symbolTable.add_constant(pair.first, pair.second);

        m_expression.register_symbol_table(m_symbolTable);

        if (!parser.compile(model, m_expression))
            throw ExpressionCompileException();
    }

    ~ExprTkModel() noexcept final override {}

    double evaluate(size_t inputSize) const final override {
        static_assert(std::numeric_limits<double>::radix == 2,
                      "Expected double type radix to be 2.");
        static_assert(std::numeric_limits<double>::digits >= 0, "");
        constexpr std::size_t max =
            sizeof(size_t) >= (std::numeric_limits<double>::digits % 8u == 0
                              ? std::numeric_limits<double>::digits / 8u
                              : std::numeric_limits<double>::digits / 8u + 1u)
            // pow(radix, digits-1) - 1
            ? (static_cast<size_t>(1) << std::numeric_limits<double>::digits)
              - 1u
            : std::numeric_limits<size_t>::max();

        if (inputSize > max)
            throw LossOfPrecisionException();

        m_inputSize = static_cast<double>(inputSize);
        return m_expression.value();
    }

private: /* Fields: */

    mutable double m_inputSize;

    exprtk::expression<double> m_expression;
    exprtk::symbol_table<double> m_symbolTable;

}; /* class ExprTkModel { */

} /* namespace { */

namespace sharemind {

SHAREMIND_DEFINE_EXCEPTION_NOINLINE(std::exception,
                                    ExecutionModelEvaluator::,
                                    Exception);
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        Exception,
        ExecutionModelEvaluator::,
        ConfigurationException,
        "Error in profiler model loader configuration!");

SHAREMIND_DEFINE_EXCEPTION_NOINLINE(std::exception,
                                    ExecutionModelEvaluator::Model::,
                                    Exception);
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        Exception,
        ExecutionModelEvaluator::Model::,
        LossOfPrecisionException,
        "Cannot cast input value without loss of precision!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        Exception,
        ExecutionModelEvaluator::Model::,
        ExpressionCompileException,
        "Failed to compile expression!");

ExecutionModelEvaluator::Model::Model() noexcept = default;
ExecutionModelEvaluator::Model::Model(Model &&) noexcept = default;
ExecutionModelEvaluator::Model::Model(Model const &) noexcept = default;
ExecutionModelEvaluator::Model::~Model() noexcept = default;

ExecutionModelEvaluator::Model & ExecutionModelEvaluator::Model::operator=(
        Model &&) noexcept = default;
ExecutionModelEvaluator::Model & ExecutionModelEvaluator::Model::operator=(
        Model const &) noexcept = default;

ExecutionModelEvaluator::ExecutionModelEvaluator(
        LogHard::Logger const & logger,
        std::string const & configFilename)
{
    using boost::property_tree::ptree;
    ptree config;

    exprtk::parser<double> parser;

    try {
        boost::property_tree::read_ini(configFilename, config);

        ptree const & baseVariables = config.get_child("BaseVariable");
        auto const inputSizeVarName(
                    baseVariables.get<std::string>("InputSize"));

        for (auto const & modelSection : config) {
            auto const & modelSectionName = modelSection.first;

            if (modelSectionName.rfind("Model") == modelSectionName.size() - 5u)
            {
                // Check for model constants
                std::map<std::string, double> constants;

                auto const constSectionName(concat(modelSectionName,
                                                   "Constant"));
                if (auto const constSection =
                            config.get_child_optional(constSectionName))
                {
                    for (auto const & constVal : *constSection) {
                        if (!constants.emplace(
                                constVal.first,
                                constVal.second.get_value<double>()).second)
                        {
                            logger.error() << "Duplicate model constants in '"
                                           << constSectionName << "'.";
                            throw ConfigurationException();
                        }
                    }
                }

                // Parse all the model expressions for this section
                auto models(makeUnique<ModelMap>());

                for (auto const & model : modelSection.second) {
                    auto const & modelName = model.first;
                    auto const modelExpression(
                                model.second.get_value<std::string>());
                    try {
                        auto model(makeUnique<ExprTkModel>(parser,
                                                           modelExpression,
                                                           inputSizeVarName,
                                                           constants));
                        if (!models->emplace(modelName,
                                             std::move(model)).second)
                        {
                            logger.error() << "Duplicate model expression "
                                              "definitions for '" << modelName
                                           << "'.";
                            throw ConfigurationException();
                        }
                    } catch (Model::ExpressionCompileException const &) {
                        logger.error() << "Invalid model expression '"
                                       << modelName << "' = '"
                                       << modelExpression << "': "
                                       << parser.error();
                        throw ConfigurationException();
                    }
                }

                if (!m_modelTypes.emplace(modelSectionName,
                                          std::move(models)).second)
                {
                    logger.error() << "Duplicate model type sections for '"
                                   << modelSectionName << "'.";
                    throw ConfigurationException();
                }
            }
        }
    } catch (boost::property_tree::ini_parser_error const & e) {
        logger.error() << "Error while parsing configuration file. "
                       << e.message() << " [" << e.filename() << ':' << e.line()
                       << "].";
        throw ConfigurationException();
    } catch (boost::property_tree::ptree_bad_data const & e) {
        logger.error() << "Error while parsing configuration file. Bad data: "
                       << e.what();
        throw ConfigurationException();
    } catch (boost::property_tree::ptree_bad_path const & e) {
        logger.error() << "Error while parsing configuration file. Bad path: "
                       << e.what();
        throw ConfigurationException();
    }
}

ExecutionModelEvaluator::Model * ExecutionModelEvaluator::model(
        std::string const & modelType,
        std::string const & modelName) const
{
    auto const modelsIt(m_modelTypes.find(modelType));
    if (modelsIt == m_modelTypes.cend())
        return nullptr;
    auto const & models = *modelsIt->second;
    auto const modelIt(models.find(modelName));
    return (modelIt != models.cend()) ? modelIt->second.get() : nullptr;
}

} /* namespace sharemind { */

/*
 * Copyright (C) 2015 Cybernetica
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
#include <LogHard/Logger.h>
#include <sharemind/compiler-support/GccPR44436.h>
#include <sstream>


namespace {

class ExprTkModel: public sharemind::ExecutionModelEvaluator::Model {

public: /* Methods: */

    inline ExprTkModel(exprtk::parser<double> & parser,
                 const std::string & model,
                 const std::string & inputSizeVariable,
                 const std::map<std::string, double> & constants)
    {
        m_symbolTable.add_variable(inputSizeVariable, m_inputSize);

        for (const auto & pair : constants)
            m_symbolTable.add_constant(pair.first, pair.second);

        m_expression.register_symbol_table(m_symbolTable);

        if (!parser.compile(model, m_expression))
            throw ExpressionCompileException();
    }

    inline ~ExprTkModel() noexcept {}

    inline double evaluate(size_t inputSize) const final override {
        static_assert(std::numeric_limits<double>::radix == 2, "Expected double type radix to be 2.");
        static_assert(std::numeric_limits<double>::digits >= 0, "");
        constexpr const size_t max =
            sizeof(size_t) >= (std::numeric_limits<double>::digits % 8u == 0
                              ? std::numeric_limits<double>::digits / 8u
                              : std::numeric_limits<double>::digits / 8u + 1u)
            // pow(radix, digits-1) - 1
            ? (static_cast<size_t>(1) << std::numeric_limits<double>::digits) - 1u
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

ExecutionModelEvaluator::ExecutionModelEvaluator(const LogHard::Logger & logger,
                                         const std::string & configFilename)
{
    using boost::property_tree::ptree;
    ptree config;

    exprtk::parser<double> parser;

    try {
        boost::property_tree::read_ini(configFilename, config);

        const ptree & baseVariables = config.get_child("BaseVariable");
        const std::string inputSizeVarName = baseVariables.get<std::string>("InputSize");

        for (const ptree::value_type & modelSection : config) {
            const std::string & modelSectionName(modelSection.first);

            if (modelSectionName.rfind("Model") ==
                    modelSectionName.size() - 5u)
            {
                // Check for model constants
                typedef std::map<std::string, double> ConstantMap;
                ConstantMap constants;

                std::ostringstream oss;
                oss << modelSectionName << "Constant";
                const std::string constSectionName(oss.str());

                boost::optional<ptree &> constSection =
                    config.get_child_optional(constSectionName);

                if (constSection) {
                    for (const ptree::value_type & constVal : *constSection) {
                        #if ! SHAREMIND_GCCPR44436
                        if (!constants.emplace(constVal.first,
                                    constVal.second.get_value<double>()).second)
                        #else
                        if (!constants.insert(ConstantMap::value_type(
                                        constVal.first,
                                        constVal.second.get_value<double>()))
                                    .second)
                        #endif
                        {
                            logger.error() << "Duplicate model constants" <<
                                " in '" << constSectionName << "'.";
                            throw ConfigurationException();
                        }
                    }
                }

                // Parse all the model expressions for this section
                std::unique_ptr<ModelMap> models(new ModelMap());

                for (const ptree::value_type & model : modelSection.second) {
                    const std::string & modelName(model.first);
                    const std::string modelExpression = model.second.get_value<std::string>();

                    try {
                        std::unique_ptr<ExprTkModel> model(
                                new ExprTkModel(parser, modelExpression,
                                                inputSizeVarName, constants));

                        #if ! SHAREMIND_GCCPR44436
                        if (!models->emplace(modelName, model.get()).second)
                        #else
                        if (!models->insert(
                                    ModelMap::value_type(modelName,
                                                         model.get())).second)
                        #endif
                        {
                            logger.error() << "Duplicate model expression" <<
                                " definitions for '" << modelName << "'.";
                            throw ConfigurationException();
                        }

                        model.release();
                    } catch (const Model::ExpressionCompileException &) {
                        logger.error() << "Invalid model expression '" << modelName
                            << "' = '" << modelExpression << "': " << parser.error();
                        throw ConfigurationException();
                    }
                }

                #if ! SHAREMIND_GCCPR44436
                if (!m_modelTypes.emplace(modelSectionName, models.get()).second)
                #else
                if (!m_modelTypes.insert(
                            ModelTypeMap::value_type(modelSectionName,
                                                     models.get())).second)
                #endif
                {
                    logger.error() << "Duplicate model type sections" <<
                        " for '" << modelSectionName << "'.";
                    throw ConfigurationException();
                }

                models.release();
            }
        }
    } catch (const boost::property_tree::ini_parser_error & e) {
        logger.error() << "Error while parsing configuration file. "
            << e.message() << " [" << e.filename() << ":"
            << e.line() << "].";
        throw ConfigurationException();
    } catch (const boost::property_tree::ptree_bad_data & e) {
        logger.error() << "Error while parsing configuration file. Bad data: "
            << e.what();
        throw ConfigurationException();
    } catch (const boost::property_tree::ptree_bad_path & e) {
        logger.error() << "Error while parsing configuration file. Bad path: "
            << e.what();
        throw ConfigurationException();
    }
}

} /* namespace sharemind { */

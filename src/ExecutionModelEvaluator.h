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

#ifndef MOD_SHARED3P_EMU_EXECUTIONMODELEVALUATOR_H
#define MOD_SHARED3P_EMU_EXECUTIONMODELEVALUATOR_H

#include <limits>
#include <LogHard/Logger.h>
#include <map>
#include <memory>
#include <sharemind/Exception.h>


namespace sharemind {

/// \todo This class should be moved to a separate facility module
class ExecutionModelEvaluator {

public: /* Types: */

    SHAREMIND_DEFINE_EXCEPTION(std::exception, Exception);
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
            Exception,
            ConfigurationException,
            "Error in profiler model loader configuration!");

    class Model {

    public: /* Types: */

        SHAREMIND_DEFINE_EXCEPTION(std::exception, Exception);
        SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
                Exception,
                LossOfPrecisionException,
                "Cannot cast input value without loss of precision!");
        SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
                Exception,
                ExpressionCompileException,
                "Failed to compile expression!");

    public: /* Methods: */

        inline virtual ~Model() noexcept {}
        virtual double evaluate(size_t inputSize) const = 0;

    }; /* class Model { */

private: /* Types: */

    using ModelMap = std::map<std::string, std::unique_ptr<Model> >;
    using ModelTypeMap = std::map<std::string, std::unique_ptr<ModelMap> >;

public: /* Methods: */

    ExecutionModelEvaluator(const LogHard::Logger & logger,
                            const std::string & configFilename);

    inline Model * model(const std::string & modelType,
                         const std::string & modelName) const
    {
        auto const modelsIt(m_modelTypes.find(modelType));
        if (modelsIt == m_modelTypes.cend())
            return nullptr;
        auto const & models = *modelsIt->second;
        auto const modelIt(models.find(modelName));
        return (modelIt != models.cend()) ? modelIt->second.get() : nullptr;
    }

private: /* Fields: */

    ModelTypeMap m_modelTypes;

}; /* class ExecutionModelEvaluator { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_EXECUTIONMODELEVALUATOR_H */

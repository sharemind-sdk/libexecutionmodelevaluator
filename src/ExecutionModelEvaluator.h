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

    SHAREMIND_DECLARE_EXCEPTION_NOINLINE(std::exception, Exception);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(Exception,
                                                   ConfigurationException);

    class Model {

    public: /* Types: */

        SHAREMIND_DECLARE_EXCEPTION_NOINLINE(std::exception, Exception);
        SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
                Exception,
                LossOfPrecisionException);
        SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
                Exception,
                ExpressionCompileException);

    public: /* Methods: */

        Model() noexcept;
        Model(Model &&) noexcept;
        Model(Model const &) noexcept;
        virtual ~Model() noexcept;

        Model & operator=(Model &&) noexcept;
        Model & operator=(Model const &) noexcept;

        virtual double evaluate(size_t inputSize) const = 0;

    }; /* class Model { */

private: /* Types: */

    using ModelMap = std::map<std::string, std::unique_ptr<Model> >;
    using ModelTypeMap = std::map<std::string, std::unique_ptr<ModelMap> >;

public: /* Methods: */

    ExecutionModelEvaluator(LogHard::Logger const & logger,
                            std::string const & configFilename);

    Model * model(std::string const & modelType,
                  std::string const & modelName) const;

private: /* Fields: */

    ModelTypeMap m_modelTypes;

}; /* class ExecutionModelEvaluator { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_EXECUTIONMODELEVALUATOR_H */

#
# Copyright (C) Cybernetica
#
# Research/Commercial License Usage
# Licensees holding a valid Research License or Commercial License
# for the Software may use this file according to the written
# agreement between you and Cybernetica.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl-3.0.html.
#
# For further information, please contact us at sharemind@cyber.ee.
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(SharemindLibExecutionModelEvaluator VERSION 0.3.0 LANGUAGES CXX)

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers 1.6 REQUIRED)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeIncludes/"
                      ${CMAKE_MODULE_PATH})
FIND_PACKAGE(ExprTk REQUIRED)
FIND_PACKAGE(LogHard 0.5.0 REQUIRED)
FIND_PACKAGE(SharemindCxxHeaders 0.8.0 REQUIRED)


# LibExecutionModelEvaluator:
FILE(GLOB_RECURSE SharemindLibExecutionModelEvaluator_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
INSTALL(FILES ${SharemindLibExecutionModelEvaluator_HEADERS}
        DESTINATION "include/sharemind"
        COMPONENT "dev")
FILE(GLOB_RECURSE SharemindLibExecutionModelEvaluator_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
SharemindAddSharedLibrary(LibExecutionModelEvaluator
    OUTPUT_NAME "sharemind_executionmodelevaluator"
    SOURCES
        ${SharemindLibExecutionModelEvaluator_SOURCES}
        ${SharemindLibExecutionModelEvaluator_HEADERS}
)
TARGET_COMPILE_DEFINITIONS(LibExecutionModelEvaluator
    PRIVATE
        "exprtk_disable_comments"
        "exprtk_disable_break_continue"
        "exprtk_disable_enhanced_features"
    )
TARGET_INCLUDE_DIRECTORIES(LibExecutionModelEvaluator
    PRIVATE
        ${ExprTk_INCLUDE_DIRS}
    INTERFACE
        # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> # TODO
        $<INSTALL_INTERFACE:include>
    )
TARGET_LINK_LIBRARIES(LibExecutionModelEvaluator
    PUBLIC
        LogHard::LogHard
        Sharemind::CxxHeaders
    )
SharemindCreateCMakeFindFilesForTarget(LibExecutionModelEvaluator
    DEPENDENCIES
        "LogHard 0.5.0"
        "SharemindCxxHeaders 0.8.0"
    )


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("lib"
    NAME "libsharemind-executionmodelevaluator"
    DESCRIPTION "Sharemind Execution Model Evaluator library"
    DEB_SECTION "libs"
    DEB_DEPENDS
        "libloghard (>= 0.5.0)"
        "libstdc++6 (>= 4.8.0)"
)
SharemindAddComponentPackage("dev"
    NAME "libsharemind-executionmodelevaluator-dev"
    DESCRIPTION "Sharemind Execution Model Evaluator library development headers"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-executionmodelevaluator (= ${SharemindLibExecutionModelEvaluator_DEB_lib_PACKAGE_VERSION})"
        "libloghard-dev (>= 0.4.0)"
        "libsharemind-cxxheaders-dev (>= 0.8.0)"
        "libstdc++-dev"
)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-executionmodelevaluator-dbg"
    DESCRIPTION "Sharemind Execution Model Evaluator debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS
        "libsharemind-executionmodelevaluator (= ${SharemindLibExecutionModelEvaluator_DEB_lib_PACKAGE_VERSION})")
SharemindPackagingFinalize()

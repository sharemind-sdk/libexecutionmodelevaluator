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
PROJECT(SharemindLibExecutionModelEvaluator VERSION 0.2.0 LANGUAGES CXX)

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers 1.2 REQUIRED)

SharemindSetProjectVersion()

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeIncludes/"
                      ${CMAKE_MODULE_PATH})
FIND_PACKAGE(ExprTk REQUIRED)
FIND_PACKAGE(LogHard REQUIRED)
FIND_PACKAGE(SharemindCxxHeaders REQUIRED)

# Headers:
FILE(GLOB_RECURSE SharemindLibExecutionModelEvaluator_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
INSTALL(FILES ${SharemindLibExecutionModelEvaluator_HEADERS}
        DESTINATION "include/sharemind"
        COMPONENT "dev")

# The library:
SharemindNewUniqueList(SharemindLibExecutionModelEvaluator_LINK_LIBRARIES
    ${LogHard_LIBRARIES}
    ${SharemindCxxHeaders_LIBRARIES}
)
SharemindNewUniqueList(SharemindLibExecutionModelEvaluator_INCLUDE_DIRS
    ${LogHard_INCLUDE_DIRS}
    ${SharemindCxxHeaders_INCLUDE_DIRS}
)
SharemindNewUniqueList(SharemindLibExecutionModelEvaluator_DEFINITIONS
    ${LogHard_DEFINITIONS}
    ${SharemindCxxHeaders_DEFINITIONS}
)
FILE(GLOB_RECURSE SharemindLibExecutionModelEvaluator_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
SharemindSetCxx11CompileOptions()
SharemindAddSharedLibrary("libexecutionmodelevaluator"
    OUTPUT_NAME "sharemind_executionmodelevaluator"
    SOURCES
        ${SharemindLibExecutionModelEvaluator_SOURCES}
        ${SharemindLibExecutionModelEvaluator_HEADERS}
    INCLUDE_DIRECTORIES
        ${SharemindLibExecutionModelEvaluator_INCLUDE_DIRS}
        ${ExprTk_INCLUDE_DIRS}
    LEGACY_DEFINITIONS
        ${SharemindLibExecutionModelEvaluator_DEFINITIONS}
    COMPILE_DEFINITIONS
        "exprtk_disable_comments"
        "exprtk_disable_break_continue"
        "exprtk_disable_enhanced_features"
    LINK_LIBRARIES
        ${SharemindLibExecutionModelEvaluator_LINK_LIBRARIES}
)

# Install CMake files:
SharemindCreateCMakeFindFiles(
    INCLUDE_DIRS
        "${CMAKE_INSTALL_PREFIX}/include"
        ${SharemindLibExecutionModelEvaluator_INCLUDE_DIRS}
    DEFINITIONS
        ${SharemindLibExecutionModelEvaluator_DEFINITIONS}
    LIBRARIES
        ${SharemindLibExecutionModelEvaluator_LINK_LIBRARIES}
        "-L${CMAKE_INSTALL_PREFIX}/lib"
        "-lsharemind_executionmodelevaluator"
)


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("lib"
    NAME "libsharemind-executionmodelevaluator"
    DESCRIPTION "Sharemind Execution Model Evaluator library"
    DEB_SECTION "libs"
    DEB_DEPENDS
        "libloghard (>= 0.1.0)"
        "libstdc++6 (>= 4.8.0)"
)
SharemindAddComponentPackage("dev"
    NAME "libsharemind-executionmodelevaluator-dev"
    DESCRIPTION "Sharemind Execution Model Evaluator library development headers"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-executionmodelevaluator (= ${SharemindLibExecutionModelEvaluator_DEB_lib_PACKAGE_VERSION})"
        "libloghard-dev (>= 0.1.0)"
        "libsharemind-cxxheaders-dev (>= 0.1.0)"
        "libstdc++-dev"
)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-executionmodelevaluator-dbg"
    DESCRIPTION "Sharemind Execution Model Evaluator debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS
        "libsharemind-executionmodelevaluator (= ${SharemindLibExecutionModelEvaluator_DEB_lib_PACKAGE_VERSION})")
SharemindPackagingFinalize()

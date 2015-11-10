#
# Copyright (C) 2015 Cybernetica
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(SharemindLibExecutionModelEvaluator CXX)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeIncludes/"
                      ${CMAKE_MODULE_PATH})

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

SET(LIBEXECUTIONMODELEVALUATOR_VERSION "0.1.0")

FIND_PACKAGE(ExprTk REQUIRED)
FIND_PACKAGE(LogHard REQUIRED)
FIND_PACKAGE(SharemindCxxHeaders REQUIRED)

SET(LIBEXECUTIONMODELEVALUATOR_EXTERNAL_LINK_LIBRARIES "")
LIST(APPEND LIBEXECUTIONMODELEVALUATOR_EXTERNAL_LINK_LIBRARIES
    ${LogHard_LIBRARIES}
    ${SharemindCxxHeaders_LIBRARIES}
)
SET(LIBEXECUTIONMODELEVALUATOR_EXTERNAL_INCLUDE_DIRS "")
LIST(APPEND LIBEXECUTIONMODELEVALUATOR_EXTERNAL_INCLUDE_DIRS
    ${ExprTk_INCLUDE_DIRS}
    ${LogHard_INCLUDE_DIRS}
    ${SharemindCxxHeaders_INCLUDE_DIRS}
)
SET(LIBEXECUTIONMODELEVALUATOR_EXTERNAL_DEFINITIONS "")
LIST(APPEND LIBEXECUTIONMODELEVALUATOR_EXTERNAL_DEFINITIONS
    ${SharemindCxxHeaders_DEFINITIONS}
    ${LogHard_DEFINITIONS}
    "-Dexprtk_disable_comments"
    "-Dexprtk_disable_break_continue"
    "-Dexprtk_disable_enhanced_features"
)
LIST(REMOVE_DUPLICATES LIBEXECUTIONMODELEVALUATOR_EXTERNAL_LINK_LIBRARIES)
LIST(REMOVE_DUPLICATES LIBEXECUTIONMODELEVALUATOR_EXTERNAL_INCLUDE_DIRS)
LIST(REMOVE_DUPLICATES LIBEXECUTIONMODELEVALUATOR_EXTERNAL_DEFINITIONS)


SET(LIBEXECUTIONMODELEVALUATOR_INSTALL_LINK_LIBRARIES
    ${LIBEXECUTIONMODELEVALUATOR_EXTERNAL_LINK_LIBRARIES}
    "-L${CMAKE_INSTALL_PREFIX}/lib" "-lsharemind_executionmodelevaluator")
SET(LIBEXECUTIONMODELEVALUATOR_INSTALL_INCLUDE_DIRS
    "${CMAKE_INSTALL_PREFIX}/include"
    ${LIBEXECUTIONMODELEVALUATOR_EXTERNAL_INCLUDE_DIRS})
SET(LIBEXECUTIONMODELEVALUATOR_INSTALL_DEFINITIONS
    ${LIBEXECUTIONMODELEVALUATOR_EXTERNAL_DEFINITIONS})
LIST(REMOVE_DUPLICATES LIBEXECUTIONMODELEVALUATOR_INSTALL_INCLUDE_DIRS)

INCLUDE_DIRECTORIES(${LIBEXECUTIONMODELEVALUATOR_EXTERNAL_INCLUDE_DIRS})
ADD_DEFINITIONS(${LIBEXECUTIONMODELEVALUATOR_EXTERNAL_DEFINITIONS})
ADD_COMPILE_OPTIONS(
  "-std=c++11" "-Wall" "-Wextra" "-O2" "-fPIC"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-ggdb>"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-fno-omit-frame-pointer>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-fomit-frame-pointer>"
)

# For enabling network statistics in the profiling results
IF(SHAREMIND_NETWORK_STATISTICS)
    ADD_DEFINITIONS("-DSHAREMIND_NETWORK_STATISTICS_ENABLE")
ENDIF(SHAREMIND_NETWORK_STATISTICS)

FILE(GLOB_RECURSE LIBEXECUTIONMODELEVALUATOR_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB_RECURSE LIBEXECUTIONMODELEVALUATOR_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
ADD_LIBRARY("libexecutionmodelevaluator" SHARED ${LIBEXECUTIONMODELEVALUATOR_SOURCES}
                                          ${LIBEXECUTIONMODELEVALUATOR_HEADERS})
SET_TARGET_PROPERTIES("libexecutionmodelevaluator" PROPERTIES
                      OUTPUT_NAME "sharemind_executionmodelevaluator"
                      VERSION "${LIBEXECUTIONMODELEVALUATOR_VERSION}"
                      SOVERSION "0.1")
TARGET_LINK_LIBRARIES("libexecutionmodelevaluator"
                      ${LIBEXECUTIONMODELEVALUATOR_EXTERNAL_LINK_LIBRARIES})
INSTALL(TARGETS "libexecutionmodelevaluator" LIBRARY DESTINATION "lib")

INSTALL(FILES ${LIBEXECUTIONMODELEVALUATOR_HEADERS} DESTINATION "include/sharemind")

FILE(GLOB_RECURSE LIBEXECUTIONMODELEVALUATOR_INFILES
     "${CMAKE_CURRENT_SOURCE_DIR}/*.in")
ADD_CUSTOM_TARGET("include_LIBEXECUTIONMODELEVALUATOR_INFILES_in_IDE"
                  SOURCES ${LIBEXECUTIONMODELEVALUATOR_INFILES})

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibExecutionModelEvaluatorConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibExecutionModelEvaluatorConfig.cmake"
  @ONLY)
INSTALL(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibExecutionModelEvaluatorConfig.cmake"
  DESTINATION "lib/SharemindLibExecutionModelEvaluator")
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibExecutionModelEvaluatorConfigVersion.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibExecutionModelEvaluatorConfigVersion.cmake"
  @ONLY)
INSTALL(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibExecutionModelEvaluatorConfigVersion.cmake"
  DESTINATION "lib/SharemindLibExecutionModelEvaluator")


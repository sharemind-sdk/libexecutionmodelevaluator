################################################################################
#
# CMake script for finding ExprTk.
# The default CMake search process is used to locate files.
#
# This script creates the following variables:
#  EXPRTK_FOUND: Boolean that indicates if the package was found
#  EXPRTK_INCLUDE_DIRS: Paths to the necessary header files
#
################################################################################

# Find headers and libraries
FIND_PATH(
    EXPRTK_INCLUDE_DIR
    NAMES
        exprtk.hpp
    HINTS
        $ENV{EXPRTK_ROOT}
        ${EXPRTK_ROOT}
    PATHS
        /usr/local
        /usr
        /opt/local
    PATH_SUFFIXES
        include
)

# Set EXPRTK_FOUND honoring the QUIET and REQUIRED arguments
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    ExprTk
    "Could NOT find ExprTk"
    EXPRTK_INCLUDE_DIR)

# Output variables
IF(EXPRTK_FOUND)
    # Include dirs
    SET(EXPRTK_INCLUDE_DIRS ${EXPRTK_INCLUDE_DIR})
ENDIF(EXPRTK_FOUND)

# Advanced options for not cluttering the cmake UIs:
MARK_AS_ADVANCED(EXPRTK_INCLUDE_DIR)

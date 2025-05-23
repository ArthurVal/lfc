cmake_minimum_required(VERSION 3.22)

project(lfc
  VERSION 0.0.1
  DESCRIPTION "TODO"
  LANGUAGES CXX
)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPERCASE)

###############################################################################
#                               GLOBAL includes                               #
###############################################################################
# We use install paths CMAKE_INSTALL_* from standard GNU install dirs See:
# https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include(GNUInstallDirs)

# cmake_print_variables()
include(CMakePrintHelpers)

# Enables other projects to depend on us, using find_package()
# configure_package_config_file() and write_basic_package_version_file()
include(CMakePackageConfigHelpers)

# Project custom
include(cmake/utils.cmake)
utils_disable_in_source_builds()
utils_if_build_type_missing(DEFAULT_TO Release)
utils_append_default_warnings_to(${PROJECT_NAME}_DEFAULT_WARNING_FLAGS)

###############################################################################
#                            GLOBAL USEFULL OPTIONS                           #
###############################################################################

# CXX STANDARD ################################################################
set(CMAKE_CXX_STANDARD 17)
cmake_print_variables(CMAKE_CXX_STANDARD)

set(CMAKE_CXX_STANDARD_REQUIRED True)
cmake_print_variables(CMAKE_CXX_STANDARD_REQUIRED)

# ENABLE_TESTING ##############################################################
# We create a single value independent of BUILD_TESTING in case we want to
# enable the unit tests only for this package. BUILD_TESTING enables unit tests
# globally.
option(${PROJECT_NAME}_ENABLE_TESTING
  "Enable unit tests build of porject \"${PROJECT_NAME}\""
  OFF
)
cmake_print_variables(
  BUILD_TESTING
  ${PROJECT_NAME}_ENABLE_TESTING
)

# BUILD_SHARED_LIBS ###########################################################
if(NOT DEFINED BUILD_SHARED_LIBS)
  message(WARNING
    " BUILD_SHARED_LIBS not defined.\n"
    " -> Setting it to OFF by default."
  )

  set(BUILD_SHARED_LIBS OFF
    CACHE BOOL "Build shared libs or not by default."
    FORCE
  )
endif()
cmake_print_variables(BUILD_SHARED_LIBS)

# CMAKE_INSTALL_PREFIX ########################################################
# Just print it, not enforced since default value is GOOD
cmake_print_variables(CMAKE_INSTALL_PREFIX)

###############################################################################
#                               CODE GENERAATION                              #
###############################################################################
# Construct config.h using CMake variables
configure_file(
  include/${PROJECT_NAME}/config.h.in
  include/${PROJECT_NAME}/config.h
  @ONLY
)

configure_file(
  include/${PROJECT_NAME}/export.h.in
  include/${PROJECT_NAME}/export.h
  @ONLY
)

###############################################################################
#                                    BUILD                                    #
###############################################################################
add_subdirectory(src)

if(BUILD_TESTING OR ${PROJECT_NAME}_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

###############################################################################
#                                   INSTALL                                   #
###############################################################################
install(
  DIRECTORY
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/>
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PATTERN "*.in" EXCLUDE
  PATTERN "*.h"
  PATTERN "*.hpp"
)

# Create and install Config and ConfigVersion.cmake  ###########################
# Create ConfigVersion.cmake, used by find_package() for version checks
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion
)

# Create the '${PROJECT_NAME}Config.cmake' using
# '${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in'
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Install files created above
install(
  FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"

  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

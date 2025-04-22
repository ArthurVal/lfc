cmake_minimum_required(VERSION 3.22)

project(lfc
  VERSION 0.0.1
  DESCRIPTION "TODO"
  LANGUAGES CXX
)

include(GNUInstallDirs)
include(cmake/utils.cmake)

utils_disable_in_source_builds()
utils_if_build_type_missing(DEFAULT_TO Release)
utils_append_default_warnings_to(${PROJECT_NAME}_DEFAULT_WARNING_FLAGS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(${PROJECT_NAME}_ENABLE_TESTING
  "Enable unit tests build of porject \"${PROJECT_NAME}\""
  OFF
)
cmake_print_variables(
  BUILD_TESTING
  ${PROJECT_NAME}_ENABLE_TESTING
)

configure_file(
  include/${PROJECT_NAME}/config.hpp.in
  include/${PROJECT_NAME}/config.hpp
  @ONLY
)

add_subdirectory(src)

install(
  DIRECTORY
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/>
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PATTERN "*.in" EXCLUDE
  PATTERN "*.hpp"
)

if(BUILD_TESTING OR ${PROJECT_NAME}_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

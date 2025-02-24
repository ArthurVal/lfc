
# From ROS2 - HUMBLE
cmake_minimum_required(VERSION 3.22)

project(lfc
  VERSION 0.0.1
  DESCRIPTION "TODO"
  LANGUAGES CXX
)

include(cmake/utils.cmake)
utils_disable_in_source_builds()
utils_if_build_type_missing(DEFAULT_TO Release)

configure_file(
  include/${PROJECT_NAME}/version.h.in
  include/${PROJECT_NAME}/version.h
  @ONLY
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

install(
  DIRECTORY
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/>
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PATTERN "*.in" EXCLUDE
  PATTERN "*.h"
)

option(${PROJECT_NAME}_ENABLE_TESTING
  "Enable unit tests build of ${PROJECT_NAME}"
  OFF
)
cmake_print_variables(${PROJECT_NAME}_ENABLE_TESTING)

if(BUILD_TESTING OR ${PROJECT_NAME}_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

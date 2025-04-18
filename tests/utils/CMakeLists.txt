add_library(${PROJECT_NAME}-tests-utils
  src/file.cpp
)

target_link_libraries(${PROJECT_NAME}-tests-utils
  PUBLIC GTest::gtest GTest::gmock
)

target_include_directories(${PROJECT_NAME}-tests-utils
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>

  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
)

target_compile_features(${PROJECT_NAME}-tests-utils
  PUBLIC
  cxx_std_17
)

target_compile_options(${PROJECT_NAME}-tests-utils
  PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -Wdouble-promotion
  -Wundef
  -Wconversion
  # -Werror
)

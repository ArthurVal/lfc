add_library(${PROJECT_NAME}-tests-utils INTERFACE)

target_link_libraries(${PROJECT_NAME}-tests-utils
  INTERFACE GTest::gtest GTest::gmock
)

target_include_directories(${PROJECT_NAME}-tests-utils
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_compile_features(${PROJECT_NAME}-tests-utils
  INTERFACE
  cxx_std_17
)

target_compile_options(${PROJECT_NAME}-tests-utils
  INTERFACE
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -Wdouble-promotion
  -Wundef
  -Wconversion
  # -Werror
)

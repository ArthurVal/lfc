add_executable(tests-${PROJECT_NAME}
  test_config.cpp
  test_linear_model.cpp
)

target_compile_definitions(tests-${PROJECT_NAME}
  PRIVATE
  PROJECT_GIT_LOCATION=${PROJECT_SOURCE_DIR}
)

target_include_directories(tests-${PROJECT_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(tests-${PROJECT_NAME}
  PRIVATE ${PROJECT_NAME}::${PROJECT_NAME}
  PRIVATE ${PROJECT_NAME}-tests-utils
  PRIVATE GTest::gtest_main
)

target_compile_options(tests-${PROJECT_NAME}
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

gtest_discover_tests(tests-${PROJECT_NAME})

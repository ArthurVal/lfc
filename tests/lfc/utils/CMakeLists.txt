add_executable(tests-${PROJECT_NAME}-utils
  test_integer_sequence.cpp
  test_tuple.cpp
)

target_link_libraries(tests-${PROJECT_NAME}-utils
  PRIVATE ${PROJECT_NAME}::utils
  PRIVATE ${PROJECT_NAME}-tests-utils
  PRIVATE GTest::gtest_main
)

gtest_discover_tests(tests-${PROJECT_NAME}-utils)

add_executable(${PROJECT_NAME}-tests
  test_pd.cpp
)

target_link_libraries(${PROJECT_NAME}-tests
  PRIVATE ${PROJECT_NAME}::${PROJECT_NAME}
  PRIVATE ${PROJECT_NAME}-tests-utils
  PRIVATE GTest::gtest_main
)

gtest_discover_tests(${PROJECT_NAME}-tests)

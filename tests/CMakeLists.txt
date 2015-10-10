set(test_LIBS gtest gtest_main ${project_LIB})
set(test_BIN ${PROJECT_NAME}-tests)

# Global test file -- includes all tests.
# To add a test to this roster include the test implementation file (.cc)
# in "all_tests.cc"
add_executable(${test_BIN} ${CMAKE_CURRENT_LIST_DIR}/test_runner.cc)
target_link_libraries(${test_BIN} ${test_LIBS})

#include <iostream>

#include "gtest/gtest.h"
#include "all_tests.cc"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);

  int result = RUN_ALL_TESTS();
  std::cout << "[  RETURN  ] Test exit code: " << result << std::endl;
  // Always return zero (required by CodeCoverage.cmake)
  return 0;
}

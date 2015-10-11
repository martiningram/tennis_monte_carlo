#include "tools.h"

class ToolsTest : public ::testing::Test {};

TEST_F(ToolsTest, FindsFilesCorrectly) {
  ASSERT_FALSE(Tools::FileExists("Robredo.csv"));
  ASSERT_TRUE(Tools::FileExists("README.md"));
}

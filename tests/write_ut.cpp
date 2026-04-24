#include <as_data_flow.h>
#include <gtest/gtest.h>
#include <write.h>

#include <sstream>
#include <vector>

TEST(WriteTest, Write) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  std::stringstream file_emulator;
  auto result = AsDataFlow(input) | Write(file_emulator, '|');
  ASSERT_EQ(file_emulator.str(), "1|2|3|4|5|");
}

TEST(WriteTest, WriteStrings) {
  std::vector<std::string> input = {"hello", "world", "foo"};
  std::stringstream ss;
  AsDataFlow(input) | Write(ss, ',');
  ASSERT_EQ(ss.str(), "hello,world,foo,");
}

TEST(WriteTest, WriteEmpty) {
  std::vector<int> input = {};
  std::stringstream ss;
  AsDataFlow(input) | Write(ss, '|');
  ASSERT_EQ(ss.str(), "");
}

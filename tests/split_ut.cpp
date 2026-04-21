#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <processing.h>

#include <fstream>
#include <sstream>
#include <vector>

struct Department {
  std::string name;

  bool operator==(const Department& other) const = default;
};

TEST(ReadTest, ByNewLine) {
  std::vector<std::stringstream> files(2);
  files[0] << "1\n2\n3\n4\n5";
  files[1] << "6\n7\n8\n9\n10";
  auto result = AsDataFlow(files) | Split("\n") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "4", "5", "6", "7",
                                           "8", "9", "10"));
}

TEST(ReadTest, ByNewLine2) {
  std::vector<std::stringstream> files(2);
  files[0] << "1\n2\n3\n\n\n4\n5";
  files[1] << "6\n7\n8\n9\n10\n";
  auto result = AsDataFlow(files) | Split("\n") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "", "", "4", "5", "6",
                                           "7", "8", "9", "10"));
}

TEST(ReadTest, BySpace) {
  std::vector<std::stringstream> files(2);
  files[0] << "1 2 3 4 5";
  files[1] << "6 7 8 9 10";
  auto result = AsDataFlow(files) | Split(" ") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "4", "5", "6", "7",
                                           "8", "9", "10"));
}

TEST(SplitTest, DelimiterAtStart) {
  std::vector<std::stringstream> files(1);
  files[0] << "\n1\n2\n3";
  auto result = AsDataFlow(files) | Split("\n") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("", "1", "2", "3"));
}

TEST(SplitTest, RawOfDelimiters) {
  std::vector<std::stringstream> files(1);
  files[0] << "1\n\n2";
  auto result = AsDataFlow(files) | Split("\n") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("1", "", "2"));
}

TEST(SplitTest, EmptyStream) {
  std::vector<std::stringstream> files(1);
  files[0] << "";
  auto result = AsDataFlow(files) | Split("\n") | AsVector();
  ASSERT_THAT(result, testing::IsEmpty());
}

TEST(SplitTest, NoDelimiter) {
  std::vector<std::stringstream> files(1);
  files[0] << "hello";
  auto result = AsDataFlow(files) | Split("\n") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("hello"));
}

TEST(SplitTest, ManyFiles) {
  std::vector<std::stringstream> files(5);
  for (int i = 0; i < 5; i++) {
    files[i] << "file" << i << "_line1\nfile" << i << "_line2";
  }
  auto result = AsDataFlow(files) | Split("\n") | AsVector();
  ASSERT_EQ(result.size(), 10);
}

TEST(SplitTest, SplitThenFilter) {
  std::vector<std::stringstream> files(1);
  files[0] << "1\n2\n3\n4\n5";
  auto result =
      AsDataFlow(files) | Split("\n") |
      Filter([](const std::string& s) { return std::stoi(s) % 2 == 0; }) |
      AsVector();
  ASSERT_THAT(result, testing::ElementsAre("2", "4"));
}

TEST(SplitTest, SplitThenTransform) {
  std::vector<std::stringstream> files(2);
  files[0] << "1\n2\n3";
  files[1] << "4\n5\n6";
  auto result = AsDataFlow(files) | Split("\n") |
                Transform([](const std::string& s) { return std::stoi(s); }) |
                AsVector();
  ASSERT_THAT(result, testing::ElementsAre(1, 2, 3, 4, 5, 6));
}

TEST(SplitTest, SplitThenFilterTransform) {
  std::vector<std::stringstream> files(1);
  files[0] << "1\n2\n3\n4\n5";
  auto result =
      AsDataFlow(files) | Split("\n") |
      Filter([](const std::string& s) { return std::stoi(s) % 2 == 0; }) |
      Transform([](const std::string& x) { return std::stoi(x); }) |
      Transform([](int x) { return x * x; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(4, 16));
}

TEST(SplitTest, SplitStrings) {
  std::vector<std::stringstream> files(1);
  files[0] << "good-department|bad department|another-good-department";

  auto result = AsDataFlow(files) | Split("|") | AsVector();

  ASSERT_THAT(result, testing::ElementsAre("good-department", "bad department",
                                           "another-good-department"));
}
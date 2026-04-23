#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <processing.h>

#include <charconv>
#include <expected>
#include <sstream>
#include <string>
#include <vector>

struct Department {
  std::string name;

  bool operator==(const Department& other) const = default;
};

std::expected<Department, std::string> ParseDepartment(const std::string& str) {
  if (str.empty()) {
    return std::unexpected("Department name is empty");
  }
  if (str.contains(' ')) {
    return std::unexpected("Department name contains space");
  }
  return Department{str};
}

std::expected<int, std::string> ParseInt(const std::string& str) {
  int value;
  auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
  if (ec != std::errc{} || ptr != str.data() + str.size())
    return std::unexpected("not a number: " + str);
  return value;
}

std::expected<int, std::string> ParsePositive(const std::string& str) {
  auto result = ParseInt(str);
  if (!result) return result;
  if (result.value() <= 0) return std::unexpected("not positive: " + str);
  return result;
}

TEST(SplitExpectedTest, SplitExpected) {
  std::vector<std::stringstream> files(1);
  files[0] << "good-department|bad department||another-good-department";

  auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") |
                                      Transform(ParseDepartment) |
                                      SplitExpected();

  std::stringstream unexpected_file;
  unexpected_flow | Write(unexpected_file, '.');

  auto expected_result = good_flow | AsVector();

  ASSERT_EQ(unexpected_file.str(),
            "Department name contains space.Department name is empty.");
  ASSERT_THAT(expected_result,
              testing::ElementsAre(Department{"good-department"},
                                   Department{"another-good-department"}));
}

TEST(SplitExpectedTest, AllValid) {
  std::vector<std::string> input = {"alpha", "beta", "gamma"};

  auto [bad, good] =
      AsDataFlow(input) | Transform(ParseDepartment) | SplitExpected();

  std::stringstream bad_out;
  bad | Write(bad_out, '|');

  auto good_result = good | AsVector();

  ASSERT_EQ(bad_out.str(), "");
  ASSERT_THAT(good_result,
              testing::ElementsAre(Department{"alpha"}, Department{"beta"},
                                   Department{"gamma"}));
}

TEST(SplitExpectedTest, AllInvalid) {
  std::vector<std::string> input = {"bad name", "", "also bad"};
  auto [bad, good] =
      AsDataFlow(input) | Transform(ParseDepartment) | SplitExpected();

  std::stringstream bad_out;
  bad | Write(bad_out, ';');

  auto good_result = good | AsVector();

  ASSERT_EQ(bad_out.str(),
            "Department name contains space;"
            "Department name is empty;"
            "Department name contains space;");
  ASSERT_THAT(good_result, testing::IsEmpty());
}

TEST(SplitExpectedTest, EmptyInput) {
  std::vector<std::string> input;
  auto [bad, good] =
      AsDataFlow(input) | Transform(ParseDepartment) | SplitExpected();

  std::stringstream bad_out;
  bad | Write(bad_out, '.');

  ASSERT_EQ(bad_out.str(), "");
  ASSERT_THAT(good | AsVector(), testing::IsEmpty());
}

TEST(SplitExpectedTest, PipelineAfterSplit) {
  std::vector<std::string> input = {"1", "abc", "-5", "3", "bad val", "10"};
  auto [bad, good] =
      AsDataFlow(input) | Transform(ParsePositive) | SplitExpected();

  auto result = good | Filter([](int x) { return x > 2; }) |
                Transform([](int x) { return x * 10; }) | AsVector();

  auto errors = bad | AsVector();

  ASSERT_THAT(result, testing::ElementsAre(30, 100));
  ASSERT_THAT(errors,
              testing::ElementsAre("not a number: abc", "not positive: -5",
                                   "not a number: bad val"));
}

TEST(SplitExpectedTest, SingleValid) {
  std::vector<std::string> input = {"finance"};
  auto [bad, good] =
      AsDataFlow(input) | Transform(ParseDepartment) | SplitExpected();

  ASSERT_THAT(good | AsVector(), testing::ElementsAre(Department{"finance"}));
  ASSERT_THAT(bad | AsVector(), testing::IsEmpty());
}

TEST(SplitExpectedTest, DuplicateErrors) {
  std::vector<std::string> input = {"", "", "ok", ""};
  auto [bad, good] =
      AsDataFlow(input) | Transform(ParseDepartment) | SplitExpected();

  ASSERT_THAT(good | AsVector(), testing::ElementsAre(Department{"ok"}));
  ASSERT_THAT(bad | AsVector(),
              testing::ElementsAre("Department name is empty",
                                   "Department name is empty",
                                   "Department name is empty"));
}

TEST(SplitExpectedTest, FromStreamWithSplit) {
  std::vector<std::stringstream> files(2);
  files[0] << "eng|hr|bad dept";
  files[1] << "|finance|also bad";

  auto [bad, good] = AsDataFlow(files) | Split("|") |
                     Transform(ParseDepartment) | SplitExpected();

  std::stringstream bad_out;
  bad | Write(bad_out, ',');

  ASSERT_EQ(bad_out.str(),
            "Department name contains space,"
            "Department name is empty,"
            "Department name contains space,");

  ASSERT_THAT(good | AsVector(),
              testing::ElementsAre(Department{"eng"}, Department{"hr"},
                                   Department{"finance"}));
}

std::expected<std::string, std::string> ParseName(const std::string& name) {
  if (std::isupper(name[0]) &&
      std::all_of(name.begin(), name.end(),
                  [](char c) { return std::isalpha(c); })) {
    return name;
  }
  return std::unexpected(name);
}

TEST(SplitExpectedTest, WorkWithSplitFlows) {
  std::vector<std::stringstream> files(3);
  files[0] << "ashdj Roma sahjsa Kekich";
  files[1] << "Lolkek 123 pipaio";
  files[2] << "illusion of safety";
  auto [wrong, names] =
      AsDataFlow(files) | Split(" ") | Transform(ParseName) | SplitExpected();

  ASSERT_THAT(names | AsVector(),
              testing::ElementsAre("Roma", "Kekich", "Lolkek"));

  auto short_words = wrong |
               Filter([](const std::string& s) { return s.size() <= 3; }) |
               AsVector();
  ASSERT_THAT(short_words, testing::ElementsAre("123", "of"));
}
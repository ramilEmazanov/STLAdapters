#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <processing.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>



TEST(FilterTest, FilterEven) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  auto result =
      AsDataFlow(input) | Filter([](int x) { return x % 2 == 0; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(2, 4));
}

TEST(FilterTest, FilterUpperCase) {
  std::vector<std::string> input = {"hello", "world", "HELLO", "WORLD"};
  auto result = AsDataFlow(input) | Filter([](const std::string& x) {
                  return std::all_of(x.begin(), x.end(),
                                     [](char c) { return std::isupper(c); });
                }) |
                AsVector();
  ASSERT_THAT(result, testing::ElementsAre("HELLO", "WORLD"));
}

// My
TEST(FilterTest, FilterThenTransform) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  auto result = AsDataFlow(input) | Filter([](int x) { return x % 2 == 0; }) |
                Transform([](int x) { return x * x; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(4, 16));
}

TEST(FilterTest, TransformThenFilter) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  auto result = AsDataFlow(input) | Transform([](int x) { return x * x; }) |
                Filter([](int x) { return x % 2 == 0; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(4, 16));
}

TEST(FilterTest, FilterStringLengthThenTransform) {
  std::vector<std::string> input = {"hello", "world", "cpp", "test"};
  auto result = AsDataFlow(input) | Filter([](const std::string& x) { return x.size() > 3; }) |
                Transform([](std::string x) { 
                  std::transform(x.begin(), x.end(), x.begin(), ::toupper); 
                  return x;
                }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("HELLO", "WORLD", "TEST"));
}

TEST(FilterTest, EmptyAfterFilter) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  auto result = AsDataFlow(input) | Filter([](int x) { return x > 5; }) | AsVector();
  ASSERT_THAT(result, testing::IsEmpty());
}

TEST(FilterTest, MultipleFilters) {
  std::vector<int> input = {1, 2, 3, 4, 5, 6};
  auto result = AsDataFlow(input) | Filter([](int x) { return x % 2 == 0; }) |
                Filter([](int x) { return x > 3; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(4, 6));
}

// Possibly TODO
// TEST(FilterTest, FilterEvenConst) {
//   const std::vector<int> input = {1, 2, 3, 4, 5};
//   auto result =
//       AsDataFlow(input) | Filter([](int x) { return x % 2 == 0; }) | AsVector();
//   ASSERT_THAT(result, testing::ElementsAre(2, 4));
// }
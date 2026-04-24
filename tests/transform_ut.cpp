#include <as_data_flow.h>
#include <as_vector.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <transform.h>
#include <filter.h>

#include <string>
#include <algorithm>
#include <vector>
#include <cctype>

auto ToUpper(std::string x) {
  std::transform(x.begin(), x.end(), x.begin(), ::toupper);
  return x;
}

struct Person {
  std::string name;
  int age;
};

TEST(TransformTest, PowerOfTwo) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  auto result =
      AsDataFlow(input) | Transform([](int x) { return x * x; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(1, 4, 9, 16, 25));
}

TEST(TransformTest, NoMutatingInitialVector) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  auto result =
      AsDataFlow(input) | Transform([](int x) { return x * x; }) | AsVector();
  ASSERT_THAT(input, testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(TransformTest, TypeConversionAfterTransform) {
  std::vector<std::string> words{"123", "345", "1243"};
  auto result =
      AsDataFlow(words) |
      Transform([](const std::string& str) { return std::stoi(str); }) |
      AsVector();
  ASSERT_THAT(result, testing::ElementsAre(123, 345, 1243));
}

TEST(TransformTest, FuncAndLambda) {
  std::vector<std::string> input = {"hello", "world", "cpp", "test"};
  auto result = AsDataFlow(input) |
                Filter([](const std::string& x) { return x.size() > 3; }) |
                Transform(ToUpper) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("HELLO", "WORLD", "TEST"));
}
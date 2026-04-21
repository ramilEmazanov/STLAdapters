#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <processing.h>

#include <optional>
#include <vector>

TEST(DropNulloptTest, DropNullopt) {
  std::vector<std::optional<int>> input = {1, std::nullopt, 3, std::nullopt, 5};
  auto result = AsDataFlow(input) | DropNullopt() | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(1, 3, 5));
}

TEST(DropNulloptTest, DropNulloptSingleNull) {
  std::vector<std::optional<int>> input = {std::nullopt};
  auto result = AsDataFlow(input) | DropNullopt() | AsVector();
  ASSERT_THAT(result, testing::IsEmpty());
}

TEST(DropNulloptTest, DropNulloptWithFilter) {
  std::vector<std::optional<int>> input = {1, 2, std::nullopt, 3,
                                           4, 5, std::nullopt};
  auto result = AsDataFlow(input) | DropNullopt() |
                Filter([](int x) { return x % 2 == 0; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(2, 4));
}

TEST(DropNulloptTest, DropNulloptWithFilterAndTransform) {
  std::vector<std::optional<int>> input = {1, 2, std::nullopt, 3,
                                           4, 5, std::nullopt};
  auto result = AsDataFlow(input) | DropNullopt() |
                Filter([](int x) { return x % 2 == 0; }) |
                Transform([](int x) { return x * x; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(4, 16));
}

TEST(DropNulloptTest, DropNulloptWithTransformAndFilter) {
  std::vector<std::optional<int>> input = {1, 2, std::nullopt, 3,
                                           4, 5, std::nullopt};
  auto result = AsDataFlow(input) | DropNullopt() |
                Transform([](int x) { return x * x; }) |
                Filter([](int x) { return x % 2 == 0; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(4, 16));
}
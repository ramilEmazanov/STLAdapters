#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <processing.h>

#include <cstddef>
#include <cstdint>
#include <set>
#include <string>
#include <utility>
#include <vector>

struct Employee {
  uint64_t department_id;
  std::string name;

  bool operator==(const Employee& other) const = default;
};

TEST(AggregateByKeyTest, CountingAggregatedValues) {
  std::vector<std::string> input = {"name4", "name0", "name1", "name0",
                                    "name2", "name0", "name1"};

  auto result =
      AsDataFlow(input) |
      AggregateByKey(
          std::size_t{0},
          [](const std::string&, std::size_t& accumulated) { ++accumulated; },
          [](const std::string& token) { return token; }) |
      AsVector();

  ASSERT_THAT(result, ::testing::ElementsAre(std::make_pair("name4", 1),
                                             std::make_pair("name0", 3),
                                             std::make_pair("name1", 2),
                                             std::make_pair("name2", 1)));
}

TEST(AggregateByKeyTest, AggregatingWithSeveralOutputsForEachKey) {
  std::vector<Employee> employees = {{3, "name1"}, {1, "name1"}, {1, "name2"},
                                     {2, "name1"}, {2, "name2"}, {2, "name3"}};

  auto result =
      AsDataFlow(employees) |
      AggregateByKey(
          std::vector<Employee>{},
          [](const Employee& employee, std::vector<Employee>& accumulated) {
            if (accumulated.size() == 2) {
              return;
            }
            accumulated.push_back(employee);
          },
          [](const Employee& employee) { return employee.department_id; }) |
      AsVector();

  ASSERT_THAT(
      result,
      ::testing::ElementsAre(
          std::make_pair(3, std::vector<Employee>{Employee{3, "name1"}}),
          std::make_pair(1, std::vector<Employee>{Employee{1, "name1"},
                                                  Employee{1, "name2"}}),
          std::make_pair(2, std::vector<Employee>{Employee{2, "name1"},
                                                  Employee{2, "name2"}})));
}

TEST(AggregateByKeyTest, EmptyInput) {
  std::vector<int> input;

  auto result = AsDataFlow(input) |
                AggregateByKey(
                    0, [](const int& key, int& value) { ++value; },
                    [](const int& v) { return v; }) |
                AsVector();

  ASSERT_TRUE(result.empty());
}

TEST(AggregateByKeyTest, AllSameKey) {
  std::vector<int> input = {5, 5, 5, 5, 5};

  auto result = AsDataFlow(input) |
                AggregateByKey(
                    0, [](const int& key, int& value) { value += key; },
                    [](const int& key) { return 42; }) |
                AsVector();

  ASSERT_THAT(result, ::testing::ElementsAre(std::make_pair(42, 25)));
}

TEST(AggregateByKeyTest, AllUnique) {
  std::vector<int> input = {1, 2, 3, 4, 5};

  auto result = AsDataFlow(input) |
                AggregateByKey(
                    0, [](const int& key, int& value) { value += 1; },
                    [](const int& key) { return key; }) |
                AsVector();

  ASSERT_THAT(result,
              ::testing::ElementsAre(std::make_pair(1, 1), std::make_pair(2, 1),
                                     std::make_pair(3, 1), std::make_pair(4, 1),
                                     std::make_pair(5, 1)));
}

TEST(AggregateByKeyTest, SetDeduplicationAccumulator) {
  std::vector<std::pair<std::string, int>> input = {
      {"a", 1}, {"b", 2}, {"a", 1}, {"a", 3}, {"b", 2}, {"b", 4}};

  auto result =
      AsDataFlow(input) |
      AggregateByKey(
          std::set<int>{},
          [](const std::pair<std::string, int>& key, std::set<int>& val) {
            val.insert(key.second);
          },
          [](const std::pair<std::string, int>& key) { return key.first; }) |
      AsVector();

  ASSERT_THAT(result,
              ::testing::ElementsAre(std::make_pair("a", std::set<int>{1, 3}),
                                     std::make_pair("b", std::set<int>{2, 4})));
}

TEST(AggregateByKeyTest, AfterTransform) {
  std::vector<int> input = {1, 2, 3, 4, 5, 6};

  auto result =
      AsDataFlow(input) | Transform([](int x) { return x * x; }) |
      AggregateByKey(
          std::vector<int>{},
          [](const int& v, std::vector<int>& val) { val.push_back(v); },
          [](const int& v) { return v % 2; }) |
      AsVector();

  ASSERT_THAT(result, ::testing::ElementsAre(
                          std::make_pair(1, std::vector<int>{1, 9, 25}),
                          std::make_pair(0, std::vector<int>{4, 16, 36})));
}

TEST(AggregateByKeyTest, AfterTransformAndFilter) {
  std::vector<int> input = {1, 2, 3, 4, 5, 6, -1, -2, -3, -4};

  auto result =
      AsDataFlow(input) | Filter([](int x) { return x > 0; }) |
      Transform([](int x) { return x * x; }) |
      AggregateByKey(
          std::vector<int>{},
          [](const int& v, std::vector<int>& val) { val.push_back(v); },
          [](const int& v) { return v % 2; }) |
      AsVector();

  ASSERT_THAT(result, ::testing::ElementsAre(
                          std::make_pair(1, std::vector<int>{1, 9, 25}),
                          std::make_pair(0, std::vector<int>{4, 16, 36})));
}

TEST(AggregateByKeyTest, NonEmptyStartingValue) {
  std::vector<int> input = {1, 2, 3, 4, 5, 6, -1, -2, -3, -4};

  auto result =
      AsDataFlow(input) | Filter([](int x) { return x > 0; }) |
      Transform([](int x) { return x * x; }) |
      AggregateByKey(
          std::vector<int>{67},
          [](const int& v, std::vector<int>& val) { val.push_back(v); },
          [](const int& v) { return v % 2; }) |
      AsVector();

  ASSERT_THAT(result, ::testing::ElementsAre(
                          std::make_pair(1, std::vector<int>{67, 1, 9, 25}),
                          std::make_pair(0, std::vector<int>{67, 4, 16, 36})));
}

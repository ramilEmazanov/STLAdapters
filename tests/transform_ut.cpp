#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <sstream>
#include <vector>

struct Person {
  std::string name;
  int age;
};

TEST(TransformTest, PowerOfTwo) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | Transform([](int x) { return x * x; }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 4, 9, 16, 25));
}

// My
TEST(TransformTest, NoMutatingInitialVector) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | Transform([](int x) { return x * x; }) | AsVector();
    ASSERT_THAT(input, testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(TransformTest, TypeConversionAfterTransform) {
    std::vector<std::string> words {"123", "345", "1243"};
    auto result = AsDataFlow(words) | Transform([](const std::string& str) { return std::stoi(str); }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(123, 345, 1243));
}

// TEST(TransformTest, TransformCustomType) {
//     std::vector<Person> people {{"a", 1}, {"b", 2}, {"c", 3}, {"d", 4}};
//     auto result = AsDataFlow(people) | Transform([](const Person& p) { return p.age > 2; }) | AsVector();
//     ASSERT_THAT(result[0], testing::ElementsAre(Person{"c", 3}, Person{"d", 4}));
// }

// TEST(TransformTest, FromStringToInt) {
//     std::vector<std::stringstream> files(2);
//     files[0] << "1 2 3 4 5";
//     files[1] << "6 7 8 9 10";
//     auto result = AsDataFlow(files) | Split(" ") | Transform([](const std::string& str) { return std::stoi(str); }) | AsVector();
//     ASSERT_THAT(result, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
// }

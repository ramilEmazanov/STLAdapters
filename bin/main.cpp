#include <as_data_flow.h>
#include <as_vector.h>
#include <processing.h>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>

#include "as_data_flow.h"
#include "filter.h"
#include "out.h"
#include "write.h"

struct Employee {
  uint64_t department_id;
  std::string name;

  bool operator==(const Employee& other) const = default;
  friend std::ostream& operator<<(std::ostream& out, const Employee& emp);
};

std::ostream& operator<<(std::ostream& out, const Employee& emp) {
  out << emp.department_id << emp.name;
  return out;
}

int main(int argc, char** argv) {
  std::vector<std::string> input = {"name4", "name0", "name1", "name0",
                                    "name2", "name0", "name1"};

  auto result =
      AsDataFlow(input) |
      AggregateByKey(
          std::size_t{0},
          [](const std::string&, std::size_t& accumulated) { ++accumulated; },
          [](const std::string& token) { return token; }) |
      AsVector();

  for (auto [key, value] : result) {
    std::cout << key << " " << value << '\n';
  }

  // ASSERT_THAT(result, ::testing::ElementsAre(std::make_pair("name4", 1),
  //                                            std::make_pair("name0", 3),
  //                                            std::make_pair("name1", 2),
  //                                            std::make_pair("name2", 1)));

  std::vector<Employee> employees = {{3, "name1"}, {1, "name1"}, {1, "name2"},
                                     {2, "name1"}, {2, "name2"}, {2, "name3"}};

  auto result2 =
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

  for (auto [key, value] : result2) {
    std::cout << key << '\n';
    for (auto x : value) {
      std::cout << x << '\n';
    }
  }

  // ASSERT_THAT(
  //     result,
  //     ::testing::ElementsAre(
  //         std::make_pair(3, std::vector<Employee>{Employee{3, "name1"}}),
  //         std::make_pair(1, std::vector<Employee>{Employee{1, "name1"},
  //                                                 Employee{1, "name2"}}),
  //         std::make_pair(2, std::vector<Employee>{Employee{2, "name1"},
  //                                                 Employee{2, "name2"}})));
  return 0;
}

// if (argc != 2) return 1;

// bool recursive = false;
// Dir(argv[1], recursive) |
//     Filter([](std::filesystem::path& p) { return p.extension() == ".txt"; })
//     | OpenFiles() | Split("\n ,.;") | Transform([](std::string& token) {
//       std::transform(token.begin(), token.end(), token.begin(),
//                      [](char c) { return std::tolower(c); });
//       return token;
//     }) |
//     AggregateByKey(
//         0uz, [](const std::string&, size_t& count) { ++count; },
//         [](const std::string& token) { return token; }) |
//     Transform([](const std::pair<std::string, size_t>& stat) {
//       return std::format("{} - {}", stat.first, stat.second);
//     }) |
//     Out(std::cout);
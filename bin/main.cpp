#include <as_data_flow.h>
#include <as_vector.h>
#include <processing.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include "as_data_flow.h"
#include "filter.h"
#include "out.h"
#include "write.h"

int main(int argc, char** argv) {
  std::vector<int> input = {1, 2, 3, 4, 5};
  auto result = AsDataFlow(input);
  for (auto x : result) {
    x += 2;
    std::cout << x << '\n';
  }
  for(auto& x : result) {
    std::cout << x << '\n';
  }
  // std::vector<int> input = {1, 2, 3, 4, 5};
  // std::stringstream file_emulator;
  // auto result = AsDataFlow(input) | Write(file_emulator, '|');

  // while (file_emulator) {
  //   std::string s;
  //   file_emulator >> s;
  //   std::cout << s;
  // }

  // for (auto x : result) {
  //   std::cout << x << ' ';
  // }
  return 0;
}

// if (agrc != 2) return 1;

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
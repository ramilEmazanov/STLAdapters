#include <as_data_flow.h>
#include <as_vector.h>
#include <processing.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <format>
#include <iostream>
#include <sstream>
#include <vector>

#include "as_data_flow.h"
#include "filter.h"
#include "out.h"
#include "write.h"

int main(int argc, char** argv) {
  if (argc != 2) return 1;

  // bool recursive = true;
  // Dir(argv[1], recursive) |
  //     Transform([](const std::filesystem::path& p) { return p.string(); }) |
  //     Out(std::cout);

  // Dir(argv[1], recursive) | Filter([](const std::filesystem::path& p) {
  //   return p.extension() == ".log";
  // }) | Transform([](std::filesystem::path& p) { return p.string(); }) |
  //     Out(std::cout);

  bool recursive = false;
  Dir(argv[1], recursive) |
      Filter([](const std::filesystem::path& p) { return p.extension() == ".log";
      }) | OpenFiles() | Split("\n") | Filter([](const std::string& line) { return
      !line.empty(); }) | Out(std::cout);

  return 0;
}

// if (argc != 2) return 1;

// bool recursive = false;
// Dir(argv[1], recursive) |
//     Filter([](std::filesystem::path& p) { return p.extension() == ".txt";
//     }) | OpenFiles() | Split("\n ,.;") | Transform([](std::string& token) {
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

// if (argc < 2) return 1;

// // Вручную формируем список путей
// std::vector<std::filesystem::path> files;
// for (int i = 1; i < argc; ++i) files.emplace_back(argv[i]);

// AsDataFlow(files) | OpenFiles() | Split("\n") |
//     AggregateByKey(
//         0uz, [](const std::string&, size_t& count) { ++count; },
//         [](const std::string& line) { return line; }) |
//     Transform([](const std::pair<std::string, size_t>& stat) {
//       return std::format("{} - {}", stat.first, stat.second);
//     }) |
//     Out(std::cout);

// return 0;
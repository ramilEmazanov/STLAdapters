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

template <typename T>
void f(T) = delete;

int main(int argc, char** argv) {


  // bool recursive = false;
  // Dir(argv[1], recursive) | Filter([](const std::filesystem::path& p) {
  //   return p.extension() == ".txt";
  // }) | OpenFiles() |
  //     Split("\n ,.;") | Transform([](std::string token) {
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
  return 0;
}
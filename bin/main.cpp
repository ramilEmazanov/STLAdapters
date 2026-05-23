#include <aggregate_by_key.h>
#include <as_data_flow.h>
#include <as_vector.h>
#include <dir.h>
#include <filter.h>
#include <merge.h>
#include <open_files.h>
#include <out.h>
#include <split.h>
#include <string.h>
#include <transform.h>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <format>
#include <iostream>
#include <utility>

/*
Программа считает частоту слов, встречающихся во всех файлах в директории
и выводит итоговую статистику в консоль
*/

int main(int argc, char** argv) {

  std::vector<int> nums = {1, 2, 1, 1, 0};
  std::vector<int> right = {1, 3, 4, 5, 6, 0};

  // std::vector<int> nums = {1, 2};
  // std::vector<int> right = {1, 3, 4, 5, 6, 0};

  // std::vector<int> nums = {1, 3, 4, 5, 6};
  // std::vector<int> right = {1, 2};

  auto flow2 = AsDataFlow(nums) | Merge(right) | AsVector();

  std::cout << "straight : ";
  for (const auto& item : flow2) {
    std::cout << item;
  }

}
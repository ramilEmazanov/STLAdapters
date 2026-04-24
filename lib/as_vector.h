#pragma once

#include <vector>

class AsVector {};

template <typename Range>
auto operator|(const Range& range, AsVector) {
  using value_type = std::decay_t<decltype(*std::declval<Range>().begin())>;

  std::vector<value_type> ans;

  for (const auto& item : range) {
    ans.push_back(item);
  }

  return ans;
}
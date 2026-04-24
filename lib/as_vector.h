#pragma once

#include <vector>

class AsVector {};

template <typename Range>
auto operator|(const Range& range, AsVector) {
  using raw_type =
      std::remove_reference_t<decltype(*std::declval<Range>().begin())>;
  using value_type = std::remove_cv_t<raw_type>;

  std::vector<value_type> ans;

  for (const auto& item : range) {
    ans.push_back(item);
  }

  return ans;
}
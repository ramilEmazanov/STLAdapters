#pragma once

class SplitExpected {};

template <typename Range>
auto operator|(const Range& range, SplitExpected) {
  using expected_type =
      std::decay_t<decltype(*std::declval<const Range>().begin())>;

  using value_type = typename expected_type::value_type;
  using error_type = typename expected_type::error_type;

  std::pair<std::vector<error_type>, std::vector<value_type>> splitted;

  for (const auto& x : range) {
    if (x.has_value()) {
      splitted.second.push_back(x.value());
    } else {
      splitted.first.push_back(x.error());
    }
  }
  return splitted;
}

class SplitExpected {};

template <typename Range>
auto operator|(const Range& range, SplitExpected) {
  using value_type = std::decay_t<
      typename decltype(*std::declval<Range>().begin())::value_type>;

  using error_type = std::decay_t<
      typename decltype(*std::declval<Range>().begin())::error_type>;

  std::pair<std::vector<error_type>, std::vector<value_type>> splitted;

  for (const auto& x : range) {
    if(x.has_value()) {
      splitted.second.push_back(x.value());
    } else {
      splitted.first.push_back(x.error());
    }
  }
  return splitted;
}
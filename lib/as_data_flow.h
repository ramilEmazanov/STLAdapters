#pragma once

#include <utility>

template <typename Container>
class AsDataFlow {
 public:
  using iterator = std::conditional_t<std::is_const_v<Container>,
                                      typename Container::const_iterator,
                                      typename Container::iterator>;

  using value_type = typename Container::value_type;

  AsDataFlow(Container& container) : container_(container) {}

  iterator begin() const { return container_.begin(); }

  iterator end() const { return container_.end(); }

 private:
  Container& container_;
};
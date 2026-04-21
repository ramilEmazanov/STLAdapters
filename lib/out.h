#pragma once

#include <ostream>

class Out {
 public:
  Out(std::ostream& out) : out_(out) {}

  template <typename Range>
  auto operator()(const Range& range) const {
    for (const auto& item : range) {
      out_ << item << '\n';
    }
  }

 private:
  std::ostream& out_;
};

// Do we need const Range& ?
// const Out& ?
template <typename Range>
void operator|(const Range& range, const Out& out) {
  out(range);
}
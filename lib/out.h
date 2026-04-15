#pragma once

#include <ostream>

class Out {
 public:
  Out(std::ostream& out) : out_(out) {}

 private:
  std::ostream& out_;
};

// Do we need const Range& ?
// const Out& ?
template<typename Range>
void operator|(const Range& range, const Out& out) {
  for(const auto& item : range) {
    out.out_ << item << '\n';
  }
}
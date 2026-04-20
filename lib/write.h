#pragma once

#include <ostream>
#include <string>

// What's with modifiers?
class WriteRange {
 public:
  WriteRange(std::ostream& out) : out_(out) {}

 private:
  std::ostream& out_;
};

class Write {
 public:
  Write(std::ostream& out, char delimiter) : delimiter_(delimiter), out_(out) {}

  template <typename Range>
  auto operator()(const Range& range) const {
    for (const auto& item : range) {
      out_ << item << delimiter_;
    }
    return WriteRange(out_);
  }

 private:
  char delimiter_;
  std::ostream& out_;
};

template <typename Range>
auto operator|(const Range& range, const Write& write) {
  return write(range);
}
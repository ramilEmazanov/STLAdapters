#pragma once

#include <ostream>
#include <string>

// What's with modifiers?
class Write {
 private:
 public:
  Write(std::ostream& out, char delimiter) : delimiter_(delimiter), out_(out) {}

  char delimiter_;
  std::ostream& out_;
};

// What to return?
template <typename Range>
Range operator|(const Range& range, const Write& write) {
  for (const auto& item : range) {
    write.out_ << item << write.delimiter_;
  }
  return range;
}
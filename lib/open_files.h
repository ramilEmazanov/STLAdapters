#pragma once

#include <fstream>
#include <memory>

class OpenFiles {};

template <typename Range>
class OpenFilesIterator {
 public:
  using base_iterator =
      std::decay_t<decltype(std::declval<const Range>().begin())>;

  OpenFilesIterator(base_iterator begin, base_iterator end)
      : current_(begin), end_(end) {
    if (begin != end) {
      cur_stream_.reset(new std::fstream(*current_));
    }
  }

  OpenFilesIterator& operator++() {
    ++current_;
    if (current_ != end_) {
      cur_stream_.reset(new std::fstream(*current_));
    }
    return *this;
  }

  std::fstream& operator*() { return *cur_stream_; }

  bool operator==(const OpenFilesIterator& other) const {
    return current_ == other.current_;
  }

  bool operator!=(const OpenFilesIterator& other) const {
    return !(*this == other);
  }

 private:
  base_iterator current_;
  base_iterator end_;
  std::shared_ptr<std::fstream> cur_stream_;
};

template <typename Range>
class OpenFilesRange {
 public:
  OpenFilesRange(const Range& range) : range_(range) {}

  auto begin() const {
    return OpenFilesIterator<Range>(range_.begin(), range_.end());
  }

  auto end() const {
    return OpenFilesIterator<Range>(range_.end(), range_.end());
  }

 private:
  Range range_;
};

template <typename Range>
auto operator|(const Range& range, OpenFiles) {
  return OpenFilesRange<Range>(range);
}

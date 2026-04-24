#pragma once

#include <fstream>
#include <memory>

class OpenFiles {};

template <typename Iterator>
class OpenFilesIterator {
 public:

  OpenFilesIterator(Iterator iter, Iterator end_iter)
      : current_(iter), end_iter_(end_iter) {
    if (iter != end_iter) {
      cur_stream_.reset(new std::fstream(*current_));
    }
  }

  OpenFilesIterator& operator++() {
    ++current_;
    if (end_iter_ != current_) {
      cur_stream_.reset(new std::fstream(*current_));
    }
    return *this;
  }

  std::fstream& operator*() const { return *cur_stream_; }

  bool operator==(const OpenFilesIterator& other) const {
    return current_ == other.current_;
  }

 private:
  Iterator current_;
  Iterator end_iter_;
  std::shared_ptr<std::fstream> cur_stream_;
};

template <typename Range>
class OpenFilesRange {
 public:
  OpenFilesRange(const Range& range) : range_(range) {}

  auto begin() const { return OpenFilesIterator(range_.begin(), range_.end()); }

  auto end() const { return OpenFilesIterator(range_.end(), range_.end()); }

 private:
  Range range_;
};

template <typename Range>
auto operator|(const Range& range, OpenFiles) {
  return OpenFilesRange<Range>(range);
}

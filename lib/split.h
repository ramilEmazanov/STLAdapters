#include <fstream>
#include <iterator>

class Split {
 public:
  Split(std::string delimiter) : delimiter_(delimiter) {}

  std::string GetDelimiter() const { return delimiter_; }

 private:
  std::string delimiter_;
};

template <typename Range>
class SplitIterator {
 public:
  using base_iterator = decltype(std::declval<Range>().begin());
  using value_type = std::string;
  using reference = const value_type&;
  using pointer = const value_type*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;

  SplitIterator(const base_iterator& begin, const base_iterator& end,
                const std::string& delimiter)
      : current_(begin), end_(end), delimiter_(delimiter) {
    ReadToken();
  }

  SplitIterator(const base_iterator& end, const std::string& delimiter, bool)
      : current_(end), end_(end), delimiter_(delimiter) {}

  bool operator==(const SplitIterator& other) const {
    return current_ == other.current_ && token_ == other.token_;
  }
  bool operator!=(const SplitIterator& other) const {
    return !(*this == other);
  }

  reference operator*() const { return token_; }
  pointer operator->() const { return &token_; }

  SplitIterator& operator++() {
    ReadToken();
    return *this;
  }
  SplitIterator operator++(int) {
    SplitIterator temp = *this;
    ++(*this);
    return temp;
  }

 private:
  void ReadToken() {
    token_.clear();

    while (current_ != end_) {
      std::istream& in = *current_;
      char c;
      while (in.get(c)) {
        if (delimiter_.contains(c)) {
          return;
        }
        token_.push_back(c);
      }
      ++current_;
      if (!token_.empty()) {
        return;
      }
    }
  }

  std::string token_;
  std::string delimiter_;
  base_iterator current_;
  base_iterator end_;
};

template <typename Range>
class SplitRange {
 public:
  SplitRange(const Range& range, const std::string& delimiter)
      : source_(range), delimiter_(delimiter) {}

  auto begin() const {
    return SplitIterator<Range>(source_.begin(), source_.end(), delimiter_);
  }
  auto end() const {
    return SplitIterator<Range>(source_.end(), delimiter_, true);
  }

 private:
  const Range& source_;
  std::string delimiter_;
};

template <typename Range>
SplitRange<Range> operator|(const Range& range, Split split) {
  return SplitRange<Range>(range, split.GetDelimiter());
}
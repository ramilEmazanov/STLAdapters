/*
Принимает стрингу, ее делит на чары и это будут делимитеры.
 1. Разделить делимитеры
 2. Хранить делимитеры
 3. В SplitIterator приходит поток
 4. Его нужно сразу разбить на вектор стрингов по делимитеру
 5. Что возвращать при разыменовании?
 6.

*/

#include <iterator>

class Split {
 public:
  Split(std::string delimiter) : delimiter_(delimiter.at(0)) {}

  char GetDelimiter() const { return delimiter_; }

 private:
  char delimiter_;
};

// How to traverse in the ifstream&
// Поэлементно проходиться и добавлять в текущий пока
template <typename Range>
class SplitIterator {
 public:
  using base_iterator = decltype(std::declval<Range>().begin());
  using value_type = std::string;
  using reference = value_type&;
  using pointer = value_type*;

  SplitIterator(const base_iterator& begin, const base_iterator& end,
                char delimiter)
      : current_(begin), end_(end), delimiter_(delimiter) {
    if (begin != end) {
      GetTokens();
    }
  }

  bool operator==(const SplitIterator& other) const {
    return current_ == other.current_;
  }

  bool operator!=(const SplitIterator& other) const {
    return !(*this == other);
  }

  value_type operator*() { return tokens_[token_index_]; }

  pointer operator->() { return &(*current_); }

  SplitIterator& operator++() {
    token_index_++;
    if (token_index_ >= tokens_.size()) {
      ++current_;
      if (current_ != end_) {
        GetTokens();
      }
    }
    return *this;
  }

  SplitIterator operator++(int) {
    SplitIterator temp = *this;
    ++(*this);
    return temp;
  }

 private:
  void GetTokens() {
    tokens_.clear();
    token_index_ = 0;
    std::istream& in = *current_;

    std::istreambuf_iterator<char> stream_begin{in};
    std::istreambuf_iterator<char> stream_end{};

    std::string unpacked(stream_begin, stream_end);

    std::string temporary{};

    for (auto value : unpacked) {
      if (value != delimiter_) {
        temporary.push_back(value);
      } else {
        tokens_.push_back(temporary);
        temporary.clear();
      }
    }

    if(!temporary.empty()) {
      tokens_.push_back(temporary);
    }

  }

  //  std::vector<std::string> delimiters_;
  size_t token_index_ = 0;
  std::vector<std::string> tokens_;

  char delimiter_;
  base_iterator current_;
  base_iterator end_;
};

template <typename Range>
class SplitRange {
 public:
  SplitRange(const Range& range, char str) : source_(range), delimiter_(str) {}

  auto begin() const {
    return SplitIterator<Range>(source_.begin(), source_.end(), delimiter_);
  }

  auto end() const {
    return SplitIterator<Range>(source_.end(), source_.end(), delimiter_);
  }

 private:
  const Range& source_;
  char delimiter_;
};

template <typename Range>
SplitRange<Range> operator|(const Range& range, Split split) {
  return SplitRange<Range>(range, split.GetDelimiter());
}
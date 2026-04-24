#include <optional>

class DropNullopt {};

template <typename Range>
class DropNulloptIterator {
 public:
  using base_iterator = decltype(std::declval<Range>().begin());
  using opt_type =
      std::remove_reference_t<decltype(*std::declval<Range>().begin())>;
  using value_type = typename opt_type::value_type;
  using reference = value_type&;
  using pointer = value_type*;

  DropNulloptIterator(const base_iterator& begin, const base_iterator& end)
      : current_(begin), end_(end) {
    advance();
  }

  DropNulloptIterator& operator++() {
    ++current_;
    advance();
    return *this;
  }

  DropNulloptIterator operator++(int) {
    DropNulloptIterator copy = *this;
    ++(*this);
    return copy;
  }

  value_type operator*() { return **current_; }

  pointer operator->() { return &(**current_); }

  bool operator==(const DropNulloptIterator& other) const {
    return current_ == other.current_;
  }

  bool operator!=(const DropNulloptIterator& other) const {
    return !(*this == other);
  }

 private:
  void advance() {
    while (current_ != end_ && *current_ == std::nullopt) {
      current_++;
    }
  }

  base_iterator current_;
  base_iterator end_;
};

template <typename Range>
class DropNulloptRange {
 public:
  DropNulloptRange(const Range& range) : source_(range) {}

  auto begin() const {
    return DropNulloptIterator<Range>(source_.begin(), source_.end());
  }

  auto end() const {
    return DropNulloptIterator<Range>(source_.end(), source_.end());
  }

 private:
  const Range& source_;
};

template <typename Range>
auto operator|(const Range& range, DropNullopt) {
  return DropNulloptRange(range);
}
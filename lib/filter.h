#pragma once

template <typename Predicate>
class Filter {
 public:
  using decayed_predicate = std::decay_t<Predicate>;

  Filter(const Predicate& predicate) : predicate_(predicate) {}

  decayed_predicate GetPredicate() const { return predicate_; }

 private:
  decayed_predicate predicate_;
};

// What about const methods, const_iterator?
template <typename Range, typename Predicate>
class FilterIterator {
 public:
  using base_iterator = decltype(std::declval<const Range&>().begin());
  using value_type =
      std::remove_reference_t<decltype(*std::declval<Range>().begin())>;
  using reference = value_type&;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using iterator_category = std::input_iterator_tag;
  using decayed_predicate = std::decay_t<Predicate>;

  FilterIterator(const Predicate& predicate, const base_iterator& begin,
                 const base_iterator& end)
      : predicate_(predicate), current_(begin), end_(end) {
    advance();
  }

  bool operator==(const FilterIterator& other) const {
    return current_ == other.current_;
  }

  bool operator!=(const FilterIterator& other) const {
    return !(*this == other);
  }

  decltype(auto) operator*() { return *current_; }

  pointer operator->() { return &(*current_); }

  FilterIterator& operator++() {
    ++current_;
    advance();
    return *this;
  }

  FilterIterator operator++(int) {
    FilterIterator temp = *this;
    ++(*this);
    return temp;
  }

 private:
  void advance() {
    while (current_ != end_ && !predicate_(*current_)) {
      ++current_;
    }
  }

  decayed_predicate predicate_;
  base_iterator current_;
  base_iterator end_;
};

template <typename Range, typename Predicate>
class FilterRange {
 public:
  using Iterator = FilterIterator<Range, Predicate>;

  FilterRange(const Range& range, const Predicate& predicate)
      : source_(range), predicate_(predicate) {}

  Iterator begin() const {
    return Iterator(predicate_, source_.begin(), source_.end());
  }

  Iterator end() const {
    return Iterator(predicate_, source_.end(), source_.end());
  }

  Predicate GetPredicate() const { return predicate_; }

 private:
  const Range& source_;
  Predicate predicate_;
};

template <typename Range, typename Predicate>
FilterRange<Range, Predicate> operator|(const Range& range,
                                        const Filter<Predicate>& filter) {
  using decayed_predicate = std::decay_t<Predicate>;

  return FilterRange<Range, decayed_predicate>(range, filter.GetPredicate());
}
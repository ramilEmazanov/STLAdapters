
template <typename Predicate>
class Transform {
 public:
  Transform(const Predicate& predicate) : predicate_(predicate) {}

  Predicate GetPredicate() const { return predicate_; }

 private:
  Predicate predicate_;
};

// Begin, end + for range based loop
template <typename Range, typename Predicate>
class TransformIterator {
 public:
  using base_iterator = decltype(std::declval<Range>().begin());
  using original_type = decltype(*std::declval<Range>().begin());
  using value_type = std::invoke_result_t<Predicate, original_type>;
  using reference = value_type&;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using iterator_category = std::input_iterator_tag;

  TransformIterator(const Predicate& predicate, const base_iterator& begin,
                    const base_iterator& end)
      : predicate_(predicate), current_(begin), end_(end) {}

  bool operator==(const TransformIterator& other) const {
    return current_ == other.current_;
  }

  bool operator!=(const TransformIterator& other) const {
    return !(*this == other);
  }
  
  decltype(auto) operator*() { return predicate_(*current_); }

  pointer operator->() { return &(*current_); }

  TransformIterator& operator++() {
    ++current_;
    return *this;
  }

  TransformIterator operator++(int) {
    TransformIterator temp = *this;
    ++(*this);
    return temp;
  }

 private:
  Predicate predicate_;
  base_iterator current_;
  base_iterator end_;
};

template <typename Range, typename Predicate>
class TransformRange {
 public:
  TransformRange(const Range& source, const Predicate& predicate)
      : source_(source), predicate_(predicate) {}

  using Iterator = TransformIterator<Range, Predicate>;

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
TransformRange<Range, Predicate> operator|(const Range& range,
                                           Transform<Predicate> transform) {
  return TransformRange<Range, Predicate>(range, transform.GetPredicate());
}
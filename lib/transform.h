#pragma once

#include <optional>

template <typename Predicate>
class Transform {
 public:
  using decayed_predicate = std::decay_t<Predicate>;

  Transform(const Predicate& predicate) : predicate_(predicate) {}

  decayed_predicate GetPredicate() const { return predicate_; }

 private:
  decayed_predicate predicate_;
};

template <typename Range, typename Predicate>
class TransformIterator {
 public:
  using base_iterator = decltype(std::declval<const Range>().begin());
  using original_type = decltype(*std::declval<Range>().begin());
  using value_type = std::invoke_result_t<Predicate, original_type>;
  using reference = value_type&;
  using pointer = value_type*;
  using decayed_predicate = std::decay_t<Predicate>;

  TransformIterator(const Predicate& predicate, const base_iterator& begin,
                    const base_iterator& end)
      : predicate_(predicate), current_(begin), end_(end) {}

  bool operator==(const TransformIterator& other) const {
    return current_ == other.current_;
  }

  bool operator!=(const TransformIterator& other) const {
    return !(*this == other);
  }

  value_type operator*() {
    if (!value_.has_value()) {
      value_ = predicate_(*current_);
    }
    return value_.value();
  }

  TransformIterator& operator++() {
    ++current_;
    value_.reset();
    return *this;
  }

  TransformIterator operator++(int) {
    TransformIterator temp = *this;
    ++(*this);
    return temp;
  }

 private:
  std::optional<value_type> value_;
  decayed_predicate predicate_;
  base_iterator current_;
  base_iterator end_;
};

template <typename Range, typename Predicate>
class TransformRange {
 public:
  using decayed_predicate = std::decay_t<Predicate>;

  TransformRange(const Range& source, const Predicate& predicate)
      : source_(source), predicate_(predicate) {}

  using Iterator = TransformIterator<Range, Predicate>;

  Iterator begin() const {
    return Iterator(predicate_, source_.begin(), source_.end());
  }

  Iterator end() const {
    return Iterator(predicate_, source_.end(), source_.end());
  }

  decayed_predicate GetPredicate() const { return predicate_; }

 private:
  Range source_;
  decayed_predicate predicate_;
};

template <typename Range, typename Predicate>
auto operator|(const Range& range, Transform<Predicate> transform) {
  using decayed_predicate = std::decay_t<Predicate>;
  return TransformRange<Range, decayed_predicate>(range,
                                                  transform.GetPredicate());
}
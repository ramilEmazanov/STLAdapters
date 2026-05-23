#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

template <typename Range>
class MergeIterator {
 public:
  using base_iterator = decltype(std::declval<const Range>().begin());

  using value_type =
      std::decay_t<decltype(*std::declval<const Range>().begin())>;

  MergeIterator(const base_iterator& begin, const base_iterator& end,
                std::map<value_type, int>& count)
      : count_(count), curr_(begin), end_(end) {
    FindNext();
  }

  bool operator==(const MergeIterator& other) const {
    return curr_ == other.curr_;
  }

  bool operator!=(const MergeIterator& other) const {
    return !(*this == other);
  }

  value_type operator*() { return *curr_; }

  MergeIterator& operator++() {
    curr_++;
    FindNext();
    return *this;
  }

  MergeIterator operator++(int) {
    MergeIterator temp = *this;
    ++(*this);
    return temp;
  }

 private:
  void FindNext() {
    while (curr_ != end_) {
      char key = *curr_;

      if (count_[key] == 0) {
        return;
      } else {
        curr_++;
        count_[key]--;
      }
    }
  }

  base_iterator curr_;
  base_iterator end_;
  std::map<value_type, int>& count_;
};

template <typename RightFlow>
class MergeTemp {
 public:
  MergeTemp(const RightFlow& rflow) : rflow_(rflow) {}

  const RightFlow& GetRightFlow() const { return rflow_; }

 private:
  const RightFlow& rflow_;
};

template <typename Range, typename RightFlow>
class MergeRange {
 public:
  using iterator = MergeIterator<Range>;
  using value_type = std::decay_t<decltype(*std::declval<Range>().begin())>;

  MergeRange(const Range& range, const RightFlow& rflow) : range_(range) {
    for (const auto& item : rflow) {
      if (count_.contains(item)) {
        count_[item] += 1;
      } else {
        count_[item] = 1;
      }
    }
  }

  iterator begin() const {
    return iterator(range_.begin(), range_.end(), count_);
  }

  iterator end() const { return iterator(range_.end(), range_.end(), count_); }

 private:
  Range range_;
  mutable std::map<value_type, int> count_;
};

template <typename RightFlow>
auto Merge(const RightFlow& rflow) {
  return MergeTemp<RightFlow>(rflow);
}

template <typename Range, typename RightFlow>
auto operator|(const Range& range, const MergeTemp<RightFlow>& merge) {
  return MergeRange<Range, RightFlow>(range, merge.GetRightFlow());
}
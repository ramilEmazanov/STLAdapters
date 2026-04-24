#pragma once

#include <iostream>
#include <optional>
#include <unordered_map>

template <typename Key, typename Value>
struct KV {
  Key key;
  Value value;
};

template <typename T>
struct is_kv : std::false_type {};

template <typename K, typename V>
struct is_kv<KV<K, V>> : std::true_type {};

template <typename T>
constexpr bool is_kv_v = is_kv<T>::value;

template <typename T>
struct kv_key {
  using type = T;
};

template <typename K, typename V>
struct kv_key<KV<K, V>> {
  using type = K;
};

template <typename T>
struct kv_value {
  using type = T;
};

template <typename K, typename V>
struct kv_value<KV<K, V>> {
  using type = V;
};

template <typename K>
using kv_key_t = typename kv_key<K>::type;

template <typename V>
using kv_value_t = typename kv_value<V>::type;

template <typename Base, typename Joined>
struct JoinResult {
  Base base;
  std::optional<Joined> joined;

  bool operator==(const JoinResult& other) const {
    return base == other.base && joined == other.joined;
  }
};

template <typename RightFlow, typename LKey, typename RKey>
class JoinTemp {
 public:
  JoinTemp(const RightFlow& rflow, const LKey& lkey, const RKey& rkey)
      : rflow_(rflow), lkey_(lkey), rkey_(rkey) {}

  using lkey_decayed = std::decay_t<LKey>;
  using rkey_decayed = std::decay_t<RKey>;

  lkey_decayed GetLKey() const { return lkey_; }

  rkey_decayed GetRKey() const { return rkey_; }

  const RightFlow& GetRightFlow() const { return rflow_; }

 private:
  const RightFlow& rflow_;
  lkey_decayed lkey_;
  rkey_decayed rkey_;
};

template <typename RightFlow>
auto Join(const RightFlow& rflow) {
  auto lkey = [](const auto& kv) { return kv.key; };
  auto rkey = lkey;
  return JoinTemp<RightFlow, decltype(lkey), decltype(rkey)>(rflow, lkey, rkey);
}

template <typename RightFlow, typename LKey, typename RKey>
auto Join(const RightFlow& rflow, const LKey& lkey, const RKey& rkey) {
  return JoinTemp<RightFlow, LKey, RKey>(rflow, lkey, rkey);
}

template <typename Range, typename LKey, typename RightMap, typename RightValue>
class JoinIterator {
 public:
  using base_iterator = decltype(std::declval<const Range>().begin());
  using raw_left_type = std::decay_t<decltype(*std::declval<base_iterator>())>;
  using left_type = kv_value_t<raw_left_type>;
  using value_type = JoinResult<left_type, RightValue>;

  JoinIterator(const base_iterator& begin, const base_iterator& end,
               const RightMap& rightmap, const LKey& lkey)
      : lkey_(lkey), map_(rightmap), current_(begin), end_(end) {}

  bool operator==(const JoinIterator& other) const {
    return current_ == other.current_;
  }

  bool operator!=(const JoinIterator& other) const { return !(*this == other); }

  value_type operator*() {
    auto item = *current_;
    auto key = lkey_(item);

    left_type base;
    if constexpr (is_kv_v<raw_left_type>) {
      base = item.value;
    } else {
      base = item;
    }

    auto it = map_.find(key);
    if (it != map_.end()) {
      return value_type{base, it->second};
    }
    return value_type{base, std::nullopt};
  }

  JoinIterator& operator++() {
    ++current_;
    return *this;
  }

  JoinIterator operator++(int) {
    JoinIterator tmp = *this;
    ++(*this);
    return tmp;
  }

 private:
  LKey lkey_;
  const RightMap& map_;
  base_iterator current_;
  base_iterator end_;
};

template <typename Range, typename RightFlow, typename LKey, typename RKey>
class JoinRange {
 public:
  using raw_left_type = std::decay_t<decltype(*std::declval<Range>().begin())>;
  using raw_right_type =
      std::decay_t<decltype(*std::declval<RightFlow>().begin())>;

  using left_type = kv_value_t<raw_left_type>;
  using right_type = kv_value_t<raw_right_type>;

  using right_key_type =
      std::decay_t<std::invoke_result_t<RKey, raw_right_type>>;

  using right_map_type = std::unordered_map<right_key_type, right_type>;
  using iterator = JoinIterator<Range, LKey, right_map_type, right_type>;

  JoinRange(const Range& range, const RightFlow& rflow, const LKey& lkey,
            const RKey& rkey)
      : range_(range), lkey_(lkey), rkey_(rkey) {
    for (const auto& item : rflow) {
      auto key = rkey_(item);

      if constexpr (is_kv_v<raw_right_type>) {
        right_map_.emplace(key, item.value);
      } else {
        right_map_.emplace(key, item);
      }

      if constexpr (is_kv_v<raw_right_type>) {
        right_map_.emplace(key, item.value);
      } else {
        right_map_.emplace(key, item);
      }
    }
  }

  iterator begin() const {
    return iterator(range_.begin(), range_.end(), right_map_, lkey_);
  }

  iterator end() const {
    return iterator(range_.end(), range_.end(), right_map_, lkey_);
  }

 private:
  Range range_;
  std::unordered_map<right_key_type, right_type> right_map_;
  LKey lkey_;
  RKey rkey_;
};

template <typename Range, typename RightFlow, typename LKey, typename RKey>
auto operator|(const Range& range,
               const JoinTemp<RightFlow, LKey, RKey>& join) {
  return JoinRange<Range, RightFlow, LKey, RKey>(
      range, join.GetRightFlow(), join.GetLKey(), join.GetRKey());
}
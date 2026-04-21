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
class JoinRange {
 public:
  JoinRange(const RightFlow& rflow, const LKey& lkey, const RKey& rkey)
      : rflow_(rflow), lkey_(lkey), rkey_(rkey) {}

  template <typename Range>
  auto operator()(const Range& range) const {
    using raw_left_type = std::decay_t<decltype(*std::begin(range))>;
    using raw_right_type = std::decay_t<decltype(*std::begin(rflow_))>;

    using left_type = kv_value_t<raw_left_type>;
    using right_type = kv_value_t<raw_right_type>;

    using right_key_type = decltype(rkey_(std::declval<raw_right_type>()));

    std::unordered_map<right_key_type, right_type> right_map;
    for (const auto& item : rflow_) {
      auto key = rkey_(item);

      if constexpr (is_kv_v<raw_right_type>) {
        right_map.emplace(key, item.value);
      } else {
        right_map.emplace(key, item);
      }
    }

    std::vector<JoinResult<left_type, right_type>> ans;
    for (const auto& item : range) {
      auto key = lkey_(item);
      auto it = right_map.find(key);

      left_type base;
      if constexpr(is_kv_v<raw_left_type>) {
        base = item.value;
      } else {
        base = item;
      }

      if (it != right_map.end()) {
        ans.push_back({base, it->second});
      } else {
        ans.push_back({base, std::nullopt});
      }
    }
    return ans;
  }

 private:
  const RightFlow& rflow_;
  LKey lkey_;
  RKey rkey_;
};

template <typename RightFlow>
auto Join(const RightFlow& rflow) {
  auto lkey = [](const auto& kv) { return kv.key; };
  auto rkey = lkey;
  return JoinRange<RightFlow, decltype(lkey), decltype(rkey)>(rflow, lkey,
                                                              rkey);
}

template <typename RightFlow, typename LKey, typename RKey>
auto Join(const RightFlow& rflow, const LKey& lkey, const RKey& rkey) {
  return JoinRange<RightFlow, LKey, RKey>(rflow, lkey, rkey);
}

template <typename Range, typename RightFlow, typename LKey, typename RKey>
auto operator|(const Range& range,
               const JoinRange<RightFlow, LKey, RKey>& join) {
  return join(range);
}
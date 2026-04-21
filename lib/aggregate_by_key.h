#include <queue>
#include <unordered_map>
#include <vector>

template <typename Value, typename Aggregator, typename KeyMaker>
class AggregateByKey {
 public:
  AggregateByKey(const Value& start_value, const Aggregator& aggregator,
                 const KeyMaker& key_maker)
      : starting_value_(start_value),
        aggregator_(aggregator),
        key_maker_(key_maker) {}

  template <typename Range>
  auto operator()(const Range& range) const {
    using element_type = std::decay_t<decltype(*std::declval<Range>().begin())>;
    using key_type = std::decay_t<std::invoke_result_t<KeyMaker, element_type>>;
    using value_type = std::decay_t<Value>;

    std::unordered_map<key_type, value_type> storage;
    std::queue<key_type> order;
    std::vector<std::pair<key_type, value_type>> ans;

    for (const auto& item : range) {
      auto key = key_maker_(item);
      order.push(key);

      if (storage.contains(key)) {
        aggregator_(item, storage[key]);
      } else {
        storage[key] = starting_value_;
        aggregator_(item, storage[key]);
      }
    }

    while (!order.empty()) {
      auto key = order.front();
      order.pop();

      if (storage.contains(key)) {
        ans.push_back({key, storage[key]});
        storage.erase(key);
      }
    }

    return ans;
  }

 private:
  Value starting_value_;
  Aggregator aggregator_;
  KeyMaker key_maker_;
};

template <typename Range, typename Value, typename Aggregator,
          typename KeyMaker>
auto operator|(const Range& range,
               const AggregateByKey<Value, Aggregator, KeyMaker>& agg) {
  return agg(range);
}
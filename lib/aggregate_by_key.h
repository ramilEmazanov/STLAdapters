#include <unordered_map>

/*
1. Для каждого элемента идем и достаем ключ по функции (проходимся по стринге,
стринга и есть ключ)
2. Ищем в мапе этот элемент. Если такого элемента в мапе нет, создаем по
start_value
3. Используем aggregator
*/

template <typename Value, typename Aggregator, typename KeyMaker>
class AggregateByKey {
 public:
  AggregateByKey(const Value& start_value, const Aggregator& aggregator,
                 const KeyMaker& key_maker)
      : starting_value_(start_value),
        aggregator_(aggregator),
        key_maker_(key_maker) {}

  using decayed_aggregator = std::decay_t<Aggregator>;
  using decayed_keymaker = std::decay_t<KeyMaker>;
  using decayed_value = std::decay_t<Value>;

  template <typename Range>
  auto operator()(const Range& range) const {
    using element_type = std::decay_t<decltype(*std::declval<Range>().begin())>;
    using key_type = std::decay_t<std::invoke_result_t<KeyMaker, element_type>>;
    using value_type = std::decay_t<Value>;

    std::unordered_map<key_type, value_type> storage;

    for (const auto& item : range) {
      auto key = key_maker_(item);

      if (storage.contains(key)) {
        aggregator_(item, storage[key]);
      } else {
        storage[key] = starting_value_;
        aggregator_(item, storage[key]);
      }
    }
    return storage;
  }

  // auto aggregate() {
  //   return std::unordered_map<int, int>;
  // }

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
  // return agg.aggregate();
}
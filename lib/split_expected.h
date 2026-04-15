class SplitExpected {};

template<typename Range>
auto operator|(Range, SplitExpected) {
  return {1,2};
}
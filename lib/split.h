class Split {
 public:
  Split(std::string delimiter) : delimiter_(delimiter) {}

 private:
  std::string delimiter_;
};

template <typename Range>
auto operator|(Range, Split) {
  return 0;
}

// How to traverse in the ifstream&
// Поэлементно проходиться и добавлять в текущий пока
class SplitIterator {};

template <typename Range>
class SplitRange {
 public:
 private:
  const Range& source_;
  std::string delimiter_;
};
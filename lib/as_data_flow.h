#pragma once

#include <utility>



// const_iterator возможно не даст пройтись используя transform
// но без const_iterator нельзя пройтись по non-const container

// template <typename Container>
// struct AsDataFlow {
//   using iterator = typename Container::const_iterator;

//   AsDataFlow(const Container& container) : container_(container) {}

//   iterator begin() const { return container_.begin(); }

//   iterator end() const { return container_.end(); }

//  private:
//   const Container& container_;
// };


template <typename Container>
struct AsDataFlow {
  using iterator = typename Container::iterator;

  AsDataFlow(Container& container) : container_(container) {}

  iterator begin() const { return container_.begin(); }

  iterator end() const { return container_.end(); }

 private:
  Container& container_;
};

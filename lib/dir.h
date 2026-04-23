#include <filesystem>

class DirIterator {
 public:
  DirIterator(const std::filesystem::recursive_directory_iterator& iter,
              bool is_rec)
      : is_recursive_(is_rec), current_(iter) {}

  std::filesystem::path operator*() { return current_->path(); }

  DirIterator& operator++() {
    if (!is_recursive_) {
      current_.disable_recursion_pending();
    }
    ++current_;
    while (current_ != std::filesystem::recursive_directory_iterator{} &&
           current_->is_directory()) {
      if (!is_recursive_) {
        current_.disable_recursion_pending();
      }
      current_++;
    }
    return *this;
  }

  DirIterator operator++(int) {
    auto copy = *this;
    ++(*this);
    return copy;
  }

  bool operator==(const DirIterator& other) const {
    return current_ == other.current_;
  }

  bool operator!=(const DirIterator& other) const { return !(*this == other); }

 private:
  bool is_recursive_;
  std::filesystem::recursive_directory_iterator current_;
};

class Dir {
 public:
  Dir(const std::string& path, bool is_rec)
      : path_(path), is_recursive_(is_rec) {}

  auto begin() const {
    return DirIterator(std::filesystem::recursive_directory_iterator{path_},
                       is_recursive_);
  }

  auto end() const {
    return DirIterator(std::filesystem::recursive_directory_iterator{},
                       is_recursive_);
  }

 private:
  std::filesystem::path path_;
  bool is_recursive_;
};

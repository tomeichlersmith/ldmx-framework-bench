/*
 *  Copyright (c), 2021, Blue Brain Project, EPFL
 *
 *  Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 */

// Compound datatype test :: May 2021
// //////////////////////////////////

#include <highfive/H5Easy.hpp>
#include <type_traits>

typedef struct {
  double width;
  double height;
  std::unordered_map<std::string, double> split() const {
    return {{"width", width}, {"height", height}};
  }
} Size2D;

class MyFile {
  H5Easy::File h5_file_;
  long unsigned int i_entry_;
 public:
  MyFile(const std::string& name = "test.h5")
      : h5_file_{name, HighFive::File::Truncate}, i_entry_{0} {}

  void next() { i_entry_++; }

  template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
  void push(std::string const& name, T const& v) {
    H5Easy::dump(h5_file_, name, v, {i_entry_});
  }

  template <typename T, std::enable_if_t<std::is_class_v<T>, bool> = true>
  void push(std::string const& name, T const& o) {
    for (auto const& [key, val] : o.split()) {
      push(name + "/" + key, val);
    }
  }
};

int main() {
  {
    MyFile file;
    std::vector<Size2D> dims = {{1., 2.5}, {3., 4.5}};
    for (std::size_t i_entry{0}; i_entry < dims.size(); i_entry++) {
      file.push("dims", dims.at(i_entry));

      /*
      std::vector<int> entries(i_entry, i_entry);
      file.push("entries", entries);
      */

      file.next();
    }
  }
}

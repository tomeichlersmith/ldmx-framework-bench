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

class MyFile {
  H5Easy::File h5_file_;
  long unsigned int i_entry_;
  long unsigned int entries_;
 public:
  class MyGroup {
    MyFile& file_;
    std::string name_;
   public:
    MyGroup(MyFile& f, std::string const& n)
      : file_{f}, name_{n} {}
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>,bool> = true>
    void save(std::string const& var_name, T const& var_val) {
      file_.save(name_+"/"+var_name, var_val);
    }
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>,bool> = true>
    T load(std::string const& var_name) const {
      return file_.load<T>(name_+"/"+var_name);
    }
  };
 public:
  MyFile(const std::string& name = "test.h5", bool write = false)
      : h5_file_{name, write ? HighFive::File::Truncate : HighFive::File::ReadOnly}, 
    i_entry_{0}, entries_{0} {

    if (not write) {
      // need to grab number of entries from specific data set
      entries_ = H5Easy::getSize(h5_file_,"i_entry");
    }
  }

  bool next() { 
    if (write) {
      i_entry_++; 
      entries_++;
    } else {
      i_entry_++;
      return i_entry_ < entries_;
    }
  }

  template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
  void save(std::string const& name, T const& v) {
    H5Easy::dump(h5_file_, name, v, {i_entry_});
  }

  template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
  T load(std::string const& name) const {
    return H5Easy::load<T>(h5_file_, name, {i_entry_});
  }

  template <typename T, std::enable_if_t<std::is_class_v<T>,bool> = true>
  void save(std::string const& name, T const& v) {
    v.save(MyGroup(*this,name));
  }

  template <typename T, std::enable_if_t<std::is_class_v<T>,bool> = true>
  T load(std::string const& name) {
    T v;
    v.load(MyGroup(*this,name));
    return v;
  }
};

struct Size2D {
  double width;
  double height;
  void save(MyFile::MyGroup g) const {
    g.save("width",width);
    g.save("height",height);
  }
  void load(MyFile::MyGroup g) {
    width = g.load<double>("width");
    height = g.load<double>("height");
  }
  bool operator!=(Size2D const& other) {
    return width != other.width or height != other.height;
  }
};

int main() {
  // dummy data to check differen types
  std::vector<Size2D> dims = {{1., 2.5}, {3., 4.5}};
  std::vector<double> areas = { 1.*2.5 , 3.*4.5 };
  std::vector<int> i_entries  = { 1 , 2 };

  std::cout << "Writing..." << std::endl;
  {
    MyFile file("test.h5",true);
    for (std::size_t i_entry{0}; i_entry < dims.size(); i_entry++) {
      file.save("dims", dims.at(i_entry));
      file.save("areas", areas.at(i_entry));
      file.save("i_entry", i_entries.at(i_entry));

      file.next();
    }
  }
  
  std::cout << "Reading..." << std::endl;
  {
    MyFile file("test.h5");
    std::size_t i_entry_actual{0};
    do {
      auto d = file.load<Size2D>("dims");
      if (d != dims.at(i_entry_actual)) {
        std::cout << "error reading dims" << std::endl;
      }

      auto a = file.load<double>("areas");
      if (a != areas.at(i_entry_actual)) {
        std::cout << "error reading area" << std::endl;
      }

      auto i = file.load<int>("i_entry");
      if (i != i_entries.at(i_entry_actual)) {
        std::cout << "error reading i_entry" << std::endl;
      }

      i_entry_actual++;
    } while (file.next());
  }
}

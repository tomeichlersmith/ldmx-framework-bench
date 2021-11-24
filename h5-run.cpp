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

/*
 * Type deduction and specialization structs for load/save
template <typename T>
struct loader {
  static T load(group,std::string const& name) {
    T ret;
    ret.load(subgroup,name);
    return ret;
  }
}

template <typename T>
using load = loader<T>::load
*/

class MyFile {
  H5Easy::File h5_file_;
  bool write_;
  long unsigned int i_entry_;
  long unsigned int entries_;
  std::unordered_map<std::string,long unsigned int>
    sub_entries_;
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
      : h5_file_{name, write ? HighFive::File::Truncate : HighFive::File::ReadOnly}, write_{write}, i_entry_{0}, entries_{0} {
    if (not write) {
      // need to grab number of entries from specific data set
      entries_ = H5Easy::getSize(h5_file_,"i_entry");
    }
  }

  bool next() { 
    i_entry_++;
    if (write_) {
      entries_++;
      return true;
    } else {
      return i_entry_ < entries_;
    }
  }

  template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
  void save(std::string const& name, T const& v) {
    std::cout << name << " saving " << i_entry_ << std::endl;
    H5Easy::dump(h5_file_, name, v, {i_entry_});
  }

  template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
  T load(std::string const& name) const {
    std::cout << name << " loading " << i_entry_ << std::endl;
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

  template <typename T, std::enable_if_t<std::is_arithmetic_v<T>,bool> = true>
  void save_vector(std::string const& name, std::vector<T> const& vec) {
    auto sub_entry_it{sub_entries_.find(name)};
    if (sub_entry_it == sub_entries_.end()) {
      sub_entries_[name] = i_entry_;
    }
    auto begin = sub_entries_.at(name);
    H5Easy::dump(h5_file_, name+"/begin", begin, {i_entry_});
    H5Easy::dump(h5_file_, name+"/end", begin+vec.size(), {i_entry_});
    for (std::size_t i_vec{0}; i_vec < vec.size(); i_vec++)
      H5Easy::dump(h5_file_, name+"/values", vec.at(i_vec), {begin+i_vec});
    sub_entries_[name] += vec.size();
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
  // for testing
  bool operator!=(Size2D const& other) {
    return width != other.width or height != other.height;
  }
};

int main() {
  // dummy data to check differen types
  std::vector<Size2D> dims = {{1., 2.5}, {3., 4.5}};
  std::vector<double> areas = { 1.*2.5 , 3.*4.5 };
  std::vector<int> i_entries  = { 1 , 2 };
  std::vector<std::vector<int>> entry_entries = {{1}, {2,2}};

  std::cout << "Writing..." << std::endl;
  {
    MyFile file("test.h5",true);
    for (std::size_t i_entry{0}; i_entry < dims.size(); i_entry++) {
      file.save("dims", dims.at(i_entry));
      file.save("areas", areas.at(i_entry));
      file.save("i_entry", i_entries.at(i_entry));
      file.save_vector("nested_entries", entry_entries.at(i_entry));

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

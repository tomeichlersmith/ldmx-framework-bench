#ifndef FIRE_H5_FILE_HPP
#define FIRE_H5_FILE_HPP

#include <highfive/H5Easy.hpp>
#include <type_traits>

namespace fire {
namespace h5 {

class File {
  H5Easy::File h5_file_;
  bool write_;
  long unsigned int i_entry_;
  long unsigned int entries_;
  std::unordered_map<std::string, long unsigned int> sub_entries_;

 public:
  class Group {
    File& file_;
    std::string name_;

   public:
    Group(File& f, std::string const& n) : file_{f}, name_{n} {}
    template <typename T,
              std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void save(std::string const& var_name, T const& var_val) {
      file_.save(name_ + "/" + var_name, var_val);
    }
    template <typename T,
              std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    T load(std::string const& var_name) const {
      return file_.load<T>(name_ + "/" + var_name);
    }
  };

 public:
  File(const std::string& name = "test.h5", bool write = false)
      : h5_file_{name,
                 write ? HighFive::File::Truncate : HighFive::File::ReadOnly},
        write_{write},
        i_entry_{0},
        entries_{0} {
    if (not write) {
      // need to grab number of entries from specific data set
      entries_ = H5Easy::getSize(h5_file_, "i_entry");
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

  template <typename T, std::enable_if_t<std::is_class_v<T>, bool> = true>
  void save(std::string const& name, T const& v) {
    v.save(Group(*this, name));
  }

  template <typename T, std::enable_if_t<std::is_class_v<T>, bool> = true>
  T load(std::string const& name) {
    T v;
    v.load(Group(*this, name));
    return v;
  }

  template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
  void save_vector(std::string const& name, std::vector<T> const& vec) {
    auto sub_entry_it{sub_entries_.find(name)};
    if (sub_entry_it == sub_entries_.end()) {
      sub_entries_[name] = i_entry_;
    }
    auto begin = sub_entries_.at(name);
    this->save(name + "/size", vec.size());
    for (std::size_t i_vec{0}; i_vec < vec.size(); i_vec++)
      H5Easy::dump(h5_file_, name + "/data", vec.at(i_vec), {begin + i_vec});
    sub_entries_[name] += vec.size();
  }

  template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
  std::vector<T> load_vector(std::string const& name) {
    auto sub_entry_it{sub_entries_.find(name)};
    if (sub_entry_it == sub_entries_.end()) {
      sub_entries_[name] = i_entry_;
    }
    auto begin = sub_entries_.at(name);
    std::size_t len{this->load<std::size_t>(name+"/size")};
    std::vector<T> vec(len);
    for (std::size_t i_vec{0}; i_vec < len; i_vec++)
      vec[i_vec] = H5Easy::load<T>(h5_file_, name + "/data", {begin + i_vec});
    sub_entries_[name] += vec.size();
  }
};  // File

}  // namespace h5
}  // namespace fire

#endif  // FIRE_H5_FILE_HPP

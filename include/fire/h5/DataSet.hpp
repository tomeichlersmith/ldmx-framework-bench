#ifndef FIRE_H5_DATASET_HPP
#define FIRE_H5_DATASET_HPP

#include <memory>
#include <type_traits>

#include <highfive/H5Easy.hpp>

namespace fire {
namespace h5 {

/// empty dataset base allowing recursion
class AbstractDataSet {
 public:
  virtual ~AbstractDataSet() = default;
  virtual void load(long unsigned int i) = 0;
  virtual void save(long unsigned int i) = 0;
};

/**
 * General data set
 */
template <typename DataType, typename Enable = void>
class DataSet : public AbstractDataSet {
 public:
  /// ignore data_handle provided for high level classes
  DataSet(H5Easy::File& f, std::string const& name, DataType* data_handle = nullptr)
      : file_{f}, name_{name}, data_handle_{std::make_unique<DataType>()} {
    data_handle_->attach(*this);
  }
  ~DataSet() = default;

  /// retrieve const reference to current data pointed to by this data set
  DataType const& get() { return *data_handle_; }

  /// update current data of this data set
  void update(DataType const& data) { *data_handle_ = data; }

  /// attach a "column" of this dataset
  template <typename ColumnType>
  void attach(std::string const& name, ColumnType& col) {
    columns_.push_back(
        std::make_unique<DataSet<ColumnType>>(file_, name_ + "/" + name, &col)
        );
  }

  void load(long unsigned int i) {
    for (auto& col : columns_) col->load(i);
  }

  void save(long unsigned int i) {
    for (auto& col : columns_) col->save(i);
  }

 private:
  /// handle to file
  H5Easy::File& file_;
  /// name of this dataset
  std::string name_;
  /// pointer to current data
  std::unique_ptr<DataType> data_handle_;
  /// list of columns in this dataset
  std::vector<std::unique_ptr<AbstractDataSet>> columns_;
};  // general data set

/**
 * Atomic types
 */
template <typename AtomicType>
class DataSet<AtomicType, std::enable_if_t<std::is_arithmetic_v<AtomicType>>>
    : public AbstractDataSet {
 public:
  /// we own atomic types when they are added directly,
  //    we won't own atomic types when they are members of higher level classes
  DataSet(H5Easy::File& f, std::string const& name, AtomicType* handle = nullptr)
      : file_{f}, name_{name}, owner_{handle == nullptr} {
    if (owner_) {
      handle_ = new AtomicType;
    } else {
      handle_ = handle;
    }
  }
  ~DataSet() {
    if (owner_) delete handle_;
  }
  AtomicType const& get() { return *handle_; }
  void update(AtomicType const& val) { *handle_ = val; }
  void load(long unsigned int i) {
    *handle_ = H5Easy::load<AtomicType>(file_, name_, {i});
  }
  void save(long unsigned int i) { H5Easy::dump(file_, name_, *handle_, {i}); }

 private:
  /// handle to file
  H5Easy::File& file_;
  /// name of this dataset
  std::string name_;
  /// we own the data
  bool owner_;
  /// pointer to current data
  AtomicType* handle_;
};

/**
 * Data set of vector types
template <typename ContentType, typename H5Easy::File>
class DataSet<std::vector<ContentType>, H5Easy::File> {
 public:
  void load() { std::size_t len; }

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
};
 */

}  // namespace h5
}  // namespace fire

#endif  // FIRE_H5_DATASET_HPP


#ifndef FIRE_H5_DATASET_HPP
#define FIRE_H5_DATASET_HPP

#include <memory>
#include <type_traits>

#include <highfive/H5Easy.hpp>

namespace fire {
namespace h5 {

/**
 * empty dataset base allowing recursion
 * this does not have the type information
 */
class BaseDataSet {
 public:
  virtual ~BaseDataSet() = default;
  virtual void load(long unsigned int i) = 0;
  virtual void save(long unsigned int i) = 0;
};

/**
 * Type-specific base class to hold common
 * dataset methods
 */
template <typename DataType>
class AbstractDataSet : public BaseDataSet {
 public:
  AbstractDataSet(H5Easy::File& f, std::string const& name, DataType* handle = nullptr)
    : file_{f}, name_{name}, owner_{handle == nullptr} {
    if (owner_) {
      handle_ = new DataType;
    } else {
      handle_ = handle;
    }
  }
  virtual ~AbstractDataSet() {
    if (owner_) delete handle_;
  }
  virtual void load(long unsigned int i) = 0;
  virtual void save(long unsigned int i) = 0;
  virtual DataType const& get() { return *handle_; }
  virtual void update(DataType const& val) {
    *handle_ = val;
  }
 protected:
  inline H5Easy::File& file() { return file_; }
  inline DataType * handle() { return handle_; }
  inline std::string const& name() const { return name_; }
  inline bool owner() { return owner_; }
 private:
  H5Easy::File& file_;
  std::string name_;
  DataType* handle_;
  bool owner_;
};

/**
 * General data set
 */
template <typename DataType, typename Enable = void>
class DataSet : public AbstractDataSet<DataType> {
 public:
  /// we own types when they are added directly (i.e. without the handle argument)
  //    we won't own types when they are members of higher level classes
  DataSet(H5Easy::File& f, std::string const& name, DataType* handle = nullptr)
      : AbstractDataSet<DataType>(f,name,handle) {
    this->handle()->attach(*this);
  }

  /// attach a "column" of this dataset
  template <typename ColumnType>
  void attach(std::string const& name, ColumnType& col) {
    columns_.push_back(
        std::make_unique<DataSet<ColumnType>>(this->file(), this->name() + "/" + name, &col)
        );
  }

  void load(long unsigned int i) {
    for (auto& col : columns_) col->load(i);
  }

  void save(long unsigned int i) {
    for (auto& col : columns_) col->save(i);
  }

 private:
  /// list of columns in this dataset
  std::vector<std::unique_ptr<BaseDataSet>> columns_;
};  // general data set

/**
 * Atomic types
 */
template <typename AtomicType>
class DataSet<AtomicType, std::enable_if_t<std::is_arithmetic_v<AtomicType>>>
    : public AbstractDataSet<AtomicType> {
 public:
  /// we own types when they are added directly (i.e. without the handle argument)
  //    we won't own types when they are members of higher level classes
  DataSet(H5Easy::File& f, std::string const& name, AtomicType* handle = nullptr)
      : AbstractDataSet<AtomicType>(f,name,handle) {}
  void load(long unsigned int i) {
    *(this->handle()) = H5Easy::load<AtomicType>(this->file(), this->name(), {i});
  }
  void save(long unsigned int i) { H5Easy::dump(this->file(), this->name(), *(this->handle()), {i}); }
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


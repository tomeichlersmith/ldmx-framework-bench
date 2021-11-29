#ifndef FIRE_H5_DATASET_HPP
#define FIRE_H5_DATASET_HPP

#include <memory>
#include <type_traits>

#include <highfive/H5Easy.hpp>

namespace fire {
namespace h5 {

/**
 * @class BaseDataSet
 *
 * Empty dataset base allowing recursion 
 *
 * This does not have the type information of the data
 * stored in any of the derived datasets, it simply instructs
 * the derived data sets to define a load and save mechanism
 * for loading/saving the dataset from/to the file.
 *
 * This type should never be seen outside of the inner-workings
 * of fire.
 */
class BaseDataSet {
 public:
  /**
   * virtual destructor so inherited classes can be
   * properly destructed.
   */
  virtual ~BaseDataSet() = default;
  /**
   * pure virtual method for loading the input entry in the data set
   */
  virtual void load(H5Easy::File& f, long unsigned int i) = 0;
  /**
   * pure virtual method for saving the input entry in the data set
   */
  virtual void save(H5Easy::File& f, long unsigned int i) = 0;
};

/**
 * @class AbstractDataSet
 *
 * Type-specific base class to hold common dataset methods.
 * 
 * Most (all I can think of?) have a shared initialization, destruction,
 * getting and setting procedure. We can house these procedures in an 
 * intermediary class in the inheritence tree.
 *
 * @tparam[in] DataType type of data being held in this set
 */
template <typename DataType>
class AbstractDataSet : public BaseDataSet {
 public:
  /**
   * Only constructor
   *
   * Passes on the handle to the file we are reading from or writing to.
   * Defines the name of the data set and the handle to the current in-memory
   * version of the data.
   *
   * If the handle is a nullptr, then we will own our own dynamically created
   * copy of the data. If the handle is not a nullptr, then we assume a parent
   * data set is holding the full object and we are simply holding a member variable,
   * so we just copy the address into our handle.
   *
   * @param[in] name name of dataset
   * @param[in] handle address of object already created (optional)
   */
  AbstractDataSet(std::string const& name, DataType* handle = nullptr)
    : name_{name}, owner_{handle == nullptr} {
    if (owner_) {
      handle_ = new DataType;
    } else {
      handle_ = handle;
    }
  }

  /**
   * Destructor
   *
   * Delete our object if we own it, otherwise do nothing.
   */
  virtual ~AbstractDataSet() {
    if (owner_) delete handle_;
  }

  /// pass on pure virtual load function
  virtual void load(H5Easy::File& f, long unsigned int i) = 0;
  /// pass on pure virtual load function
  virtual void save(H5Easy::File& f, long unsigned int i) = 0;

  /**
   * Get the current in-memory data object
   *
   * @note virtual so that derived data sets
   * could specialize this, but I can't think of a reason to do so.
   *
   * @return const reference to current data
   */
  virtual DataType const& get() { return *handle_; }

  /**
   * Update the in-memory data object with the passed value.
   *
   * @note virtual so that derived data sets
   * could specialize this, but I can't think of a reason to do so.
   *
   * @param[in] val new value the in-memory object should be
   */
  virtual void update(DataType const& val) {
    *handle_ = val;
  }

 protected:
  /// name of data set
  std::string name_;
  /// handle on current object in memory
  DataType* handle_;
  /// we own the object in memory
  bool owner_;
};  // AbstractDataSet

/**
 * General data set
 *
 * This is the top-level data set that will be used most often.
 * It is meant to be used by a class which registers its member
 * variables to this set via its 'attach' method.
 *
 * ```cpp
 * class MyData {
 *  public:
 *   MyData() = default; // required by serialization technique
 *   // other public members
 *   void attach(fire::h5::DataSet<MyData>& set) {
 *     set.attach("my_double",my_double_);
 *   }
 *  private:
 *   double my_double_;
 * };
 * ```
 */
template <typename DataType, typename Enable = void>
class DataSet : public AbstractDataSet<DataType> {
 public:
  /**
   * Default constructor
   *
   * After the intermediate class AbstractDataSet does the
   * initialization, we call the 'attach' method of the data
   * pointed to by our handle. This allows us to register
   * its member variables with our own 'attach' method.
   */
  DataSet(std::string const& name, DataType* handle = nullptr)
      : AbstractDataSet<DataType>(name,handle) {
    this->handle_->attach(*this);
  }

  /**
   * Loading this dataset from the file involves simply loading
   * all of the members of the data type.
   */
  void load(H5Easy::File& f, long unsigned int i) {
    for (auto& m : members_) m->load(f,i);
  }

  /*
   * Saving this dataset from the file involves simply saving
   * all of the members of the data type.
   */
  void save(H5Easy::File& f, long unsigned int i) {
    for (auto& m : members_) m->save(f,i);
  }

  /**
   * Attach a member object from the our data handle
   *
   * We create a new child DataSet so that we can recursively
   * handle complex member variable types.
   *
   * @tparam[in] MemberType type of member variable we are attaching
   * @param[in] name name of member variable
   * @param[in] m reference of member variable
   */
  template <typename MemberType>
  void attach(std::string const& name, MemberType& m) {
    members_.push_back(
        std::make_unique<DataSet<MemberType>>(this->name_ + "/" + name, &m)
        );
  }

 private:
  /// list of members in this dataset
  std::vector<std::unique_ptr<BaseDataSet>> members_;
};  // DataSet

/**
 * Atomic types
 *
 * Once we finally recurse down to actual fundamental ("atomic") types,
 * we can start actually calling the file load and save methods.
 *
 * @TODO port H5Easy::File into our framework. We can specialize it
 * to our purposes to improve performance.
 */
template <typename AtomicType>
class DataSet<AtomicType, std::enable_if_t<std::is_arithmetic_v<AtomicType>>>
    : public AbstractDataSet<AtomicType> {
 public:
  /**
   * We don't do any more initialization except which is handled by the AbstractDataSet
   */
  DataSet(std::string const& name, AtomicType* handle = nullptr)
      : AbstractDataSet<AtomicType>(name,handle) {}
  /**
   * Call the H5Easy::load method with our atomic type and our name
   */
  void load(H5Easy::File& f, long unsigned int i) {
    *(this->handle_) = H5Easy::load<AtomicType>(f, this->name_, {i});
  }
  /**
   * Call the H5Easy::save method with our atomic type and our name
   */
  void save(H5Easy::File& f, long unsigned int i) { H5Easy::dump(f, this->name_, *(this->handle_), {i}); }
};  // DataSet<AtomicType>

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


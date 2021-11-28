#ifndef FIRE_H5_DATASET_HPP
#define FIRE_H5_DATASET_HPP

#include <memory>

namespace fire {
namespace h5 {

/**
 * General data set
 */
template <
  typename DataType,
  typename FileType
  >
class DataSet {
 public:
  DataSet(FileType& f, std::string const& name)
    : file_{f}, name_{name}, data_handle_{std::make_unique<DataType>()} {}
  ~DataSet() = default;

  /// retrieve const reference to current data pointed to by this data set
  DataType const& get() {
    return *data_handle_;
  }

  /// update current data of this data set
  void update(DataType const& data) {
    *data_handle_ = data;
  }

  /// load the next entry of this dataset into the data handle
  void load() {
    data_handle_->load(*this);
  }

  /// save the data handle into this data set
  void save() {
    data_handle_->save(*this);
  }

 private:
  FileType& file_;
  std::string name_;
  /// pointer to current data
  std::unique_ptr<DataType> data_handle_;
};  // general data set

/**
 * Data set of vector types
 */
template <
  typename ContentType,
  typename FileType
  >
class DataSet<std::vector<ContentType>,FileType> {
 public:

   void load() {
    std::size_t len;
    
   }

};

}
}

#endif // FIRE_H5_DATASET_HPP


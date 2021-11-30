#ifndef FIRE_H5_FILE_HPP
#define FIRE_H5_FILE_HPP

// use HDF5 C API
#include <hdf5.h>

namespace fire {
namespace h5 {

/**
 * An HDF5 file specialed to fire's usecase
 */
class File {
 public:
  File(std::string const& name, bool write) : writing_{write} {
    if (writing_) {
      file_id_ =
          H5Fcreate(name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    } else {
      file_id_ =
          H5Fopen(name.c_str(), H5F_ACC_READONLY, H5P_DEFAULT, H5P_DEFAULT);
    }
  }

  ~File() {
    if (writing_) {
      if ((status_ = H5Fflust(file_id_)) < 0) {
        throw std::runtime_error("H5 file closure failed with error code " +
                                 std::to_string(status_));
      }
    }
    if ((status_ = H5Fclose(file_id_)) < 0) {
      throw std::runtime_error("H5 file closure failed with error code " +
                               std::to_string(status_));
    }
  }

  template <
      typename AtomicType,
      std::enable_if_t<std::is_arithmetic_v<AtomicType>, bool >= true> void
          load(const std::string& path, long unsigned int i, AtomicType* val) {
    if (writing_) {
      // should never call 'load' on an output file
      throw std::runtime_error("Attempted to load data from the output file.");
    }
    if (id_.find(path) == id_.end()) {
      // initialize by opening dataset from our file
      id_[path] = H5Dopen(file_id_, path.c_str(), /*access properties*/);
    }
    /**
     * select the hyperslab to read from and then do the read
     *  H5Dread and H5Sselect_hyperslab
     *
     * id_.at(path) is the hid_t of the DataSet to load
     */
  }

  template <
      typename AtomicType,
      std::enable_if_t<std::is_arithmetic_v<AtomicType>, bool >= true> void
          save(const std::string& path, long unsigned int i, AtomicType* val) {
    if (not writing_) {
      throw std::runtime_error("Attempted to save data to the input file.");
    }
    if (id_.find(path) == id_.end()) {
      // initialize by creating a dataset to write to
      id_[path] = H5Dcreate(file_id_, path.c_str(), /*access properties*/);
    }
    /**
     * select the hyperslab to write to and then do the write
     *  H5Dwrite and H5Sselect_hyperslab
     *
     * id_.at(path) is the hid_t of the DataSet to write
     */
  }

 private:
  /// file handle
  hid_t file_id_;
  /// we are writing
  bool writing_;
  /// Cache of HDF5 C-API handles already in memory
  std::unorderd_map<std::string, hid_t> id_;
  /// HDF5 C-API status object
  mutable herr_t status_;
};

}  // namespace h5
}  // namespace fire

#endif  // FIRE_H5_FILE_HPP

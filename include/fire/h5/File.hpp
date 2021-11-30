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
  File(std::string const& name, bool write) {
    if (write) {
      file_id_ = H5Fcreate(name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    } else {
      file_id_ = H5Fcreate(name.c_str(), H5F_ACC_READONLY, H5P_DEFAULT, H5P_DEFAULT);
    }
  }

  ~File() {
    if((status_ = H5Fclose(file_id_)) < 0) {
      throw std::runtime_error("H5 file closure failed with error code "+std::to_string(status_));
    }
  }

 private:
  /// HDF5 C-API handle to file
  hid_t file_id_;
  /// HDF5 C-API status object
  mutable herr_t status_;
};

}
}

#endif  // FIRE_H5_FILE_HPP

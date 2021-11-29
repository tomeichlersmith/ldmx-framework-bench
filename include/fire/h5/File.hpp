#ifndef FIRE_H5_FILE_HPP
#define FIRE_H5_FILE_HPP

#include <exception>
#include "fire/h5/DataSet.hpp"

namespace fire {
namespace h5 {

class File {
  H5Easy::File h5_file_;
  bool write_;
  long unsigned int i_entry_;
  long unsigned int entries_;
  std::unordered_map<std::string, std::unique_ptr<BaseDataSet>> sets_;

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

  bool next(bool should_save = true) {
    i_entry_++;
    if (write_) {
      if (should_save) {
        for (auto& [_, set] : sets_) set->save(i_entry_ - 1);
      }
      entries_++;
      return true;
    } else {
      if (i_entry_ < entries_) {
        for (auto& [_, set] : sets_) set->load(i_entry_);
        return true;
      } else
        return false;
    }
  }

  template <typename DataType>
  void add(std::string const& name, DataType& data) {
    auto set_it{sets_.find(name)};
    if (set_it == sets_.end()) {
      // TODO prevent two add's of same 'name' per event
      sets_[name] = std::make_unique<DataSet<DataType>>(h5_file_, name);
    }
    try {
      /// maybe throw bad_cast exception
      dynamic_cast<DataSet<DataType>&>(*sets_[name]).update(data);
    } catch (std::bad_cast const&) {
      throw std::runtime_error("DataSet corresponding to " + name +
                           " has different type.");
    }
  }

  template <typename DataType>
  DataType const& get(std::string const& name) {
    auto set_it{sets_.find(name)};
    if (set_it == sets_.end()) {
      // check if file on disk by trying to create and load it
      //  this line won't throw an error because we haven't tried accessing the data yet
      sets_[name] = std::make_unique<DataSet<DataType>>(h5_file_, name);
      //  this line may throw an error
      sets_[name]->load(i_entry_);
    }

    // type casting, 'bad_cast' thrown if unable
    try {
      return dynamic_cast<DataSet<DataType>&>(*sets_[name]).get();
    } catch (std::bad_cast const&) {
      throw std::runtime_error("DataSet corresponding to " + name +
                           " has different type.");
    }
  }
};  // File

}  // namespace h5
}  // namespace fire

#endif  // FIRE_H5_FILE_HPP

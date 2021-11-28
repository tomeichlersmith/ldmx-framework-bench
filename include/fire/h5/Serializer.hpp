#ifndef FIRE_H5_SERIALIZER_HPP
#define FIRE_H5_SERIALIZER_HPP

namespace fire {
namespace h5 {

/*
 * Default serializer is for all user classes
 *  we assume that user classes have two member
 *  functions defined with specific signatures.
 */
template <
  typename UserDataType,
  typename FileType
  >
struct serializer {
  static void load(FileType& f, std::string const& path, UserDataType& d) {
    d.load(f,path);
  }
  static void save(FileType& f, std::string const& path, UserDataType const& d) {
    d.save(f,path);
  }
};

/**
 * Specialization of vector tyes
 */
template <
  typename ContentType,
  typename FileType
  >
struct serializer<std::vector<ContentType>,FileType> {
  static void load(FileType& f, std::string const& path, std::vector<ContentType>& d) {
    std::size_t len;
    serializer<std::size_t,FileType>(f,path+"/size",len);
    d.resize(len);
    for (ContentType& entry : d)
      serializer<ContentType,FileType>(f,path+"/data",entry);
  }
  static void save(FileType& f, std::string const& path, std::vector<ContentType> const& d) {
    serializer<std::size_t,FileType>(f,path+"/size",d.size());
    for (ContentType const& entry : d)
      serializer<ContentType,FileType>(f,path+"/data",entry);
  }
};

}  // namespace h5
}  // namespace fire

#endif

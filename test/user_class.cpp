#include <boost/test/tools/interface.hpp>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "fire/h5/File.hpp"
#include "fire/h5/DataSet.hpp"

// test a plain old data user class
class Size2D {
  double width;
  double height;
 public:
  Size2D() = default; // empty, default constructor necessary for serialization
  Size2D(double w, double h) : width(w), height(h) {}
  // for testing
  bool operator==(Size2D const& other) {
    return width == other.width and height == other.height;
  }
 private:
  friend class fire::h5::DataSet<Size2D>;
  void attach(fire::h5::DataSet<Size2D> &set) {
    set.attach("width",width);
    set.attach("height",height);
  }
};

// test a user class that is a member of a user class
class Size3D {
  double depth;
  Size2D xsec;
 public:
  Size3D() = default; // required for serialization
  Size3D(double w, double h, double d) : xsec(w,h), depth(d) {}
  // for testing
  bool operator==(Size3D const& other) {
    return xsec == other.xsec and depth == other.depth;
  }
 private:
  friend class fire::h5::DataSet<Size3D>;
  void attach(fire::h5::DataSet<Size3D> &set) {
    set.attach("depth",depth);
    set.attach("xsec",xsec);
  }
};

BOOST_AUTO_TEST_CASE(user_class) {
  std::string filename{"user_class.h5"};
  std::vector<Size2D> dims;
  dims.emplace_back(1.,2.5);
  dims.emplace_back(3.,4.5);
  dims.emplace_back(-9.8,-420.);

  std::vector<Size3D> nested;
  nested.emplace_back(3.0,2.,1);
  nested.emplace_back(-3.0,20.,0.1);
  nested.emplace_back(98.,32.,-11);

  try { // Writing
    fire::h5::File file(filename,true);
    for (std::size_t i_entry{0}; i_entry < dims.size(); i_entry++) {
      file.add("dims", dims.at(i_entry));
      file.add("nested", nested.at(i_entry));

      // needed for event counting, standing in for event header
      file.add("i_entry", i_entry);
      file.next();
    }
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    BOOST_REQUIRE(false);
  }
  
  try { // Reading
    fire::h5::File file(filename);
    std::size_t i_entry{0};
    do {
      auto d = file.get<Size2D>("dims");
      BOOST_CHECK(d == dims.at(i_entry));

      auto s = file.get<Size3D>("nested");
      BOOST_CHECK(s == nested.at(i_entry));

      i_entry++;
    } while (file.next());
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    BOOST_REQUIRE(false);
  }
}

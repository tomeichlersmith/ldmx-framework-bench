#include <boost/test/tools/interface.hpp>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "fire/h5/File.hpp"
#include "fire/h5/DataSet.hpp"

struct Size2D {
  double width;
  double height;
  void attach(fire::h5::DataSet<Size2D> &set) {
    set.attach("width",width);
    set.attach("height",height);
  }
  // for testing
  bool operator==(Size2D const& other) {
    return width == other.width and height == other.height;
  }
};

BOOST_AUTO_TEST_CASE(user_class) {
  std::string filename{"user_class.h5"};
  std::vector<Size2D> dims = {{1., 2.5}, {3., 4.5}, {-9.8, -420.}};

  try { // Writing
    fire::h5::File file(filename,true);
    for (std::size_t i_entry{0}; i_entry < dims.size(); i_entry++) {
      file.add("dims", dims.at(i_entry));

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

      i_entry++;
    } while (file.next());
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    BOOST_REQUIRE(false);
  }
}

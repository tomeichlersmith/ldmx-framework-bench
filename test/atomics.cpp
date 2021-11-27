#include <boost/test/tools/interface.hpp>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "fire/h5/File.hpp"

BOOST_AUTO_TEST_CASE(atomics) {
  std::string filename{"atomics.h5"};

  std::vector<double> doubles = { 1.0, 32., 69., 420., -9.0 };
  std::vector<int>    ints    = { 0, -33, 88, 39, 123 };
  std::vector<bool>   bools   = { true, false, false, true, true };

  BOOST_CHECK_NO_THROW({ // Writing
    fire::h5::File file(filename,true);
    for (std::size_t i_entry{0}; i_entry < doubles.size(); i_entry++) {
      file.save("double", doubles.at(i_entry));
      file.save("int", ints.at(i_entry));
      file.save("bool", bool(bools.at(i_entry)));

      // needed for event counting, standing in for event header
      file.save("i_entry", i_entry);
      file.next();
    }
  });

  BOOST_CHECK_NO_THROW({ // Reading
    fire::h5::File file(filename);
    std::size_t i_entry{0};
    do {
      auto d = file.load<double>("double");
      BOOST_CHECK(d == doubles.at(i_entry));

      auto i = file.load<int>("int");
      BOOST_CHECK(i == ints.at(i_entry));

      auto b = file.load<bool>("bool");
      BOOST_CHECK(b == bools.at(i_entry));

      i_entry++;
    } while(file.next());
  });
}

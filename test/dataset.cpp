#include <boost/test/tools/interface.hpp>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "fire/Process.hpp"

BOOST_AUTO_TEST_CASE(atomics) {
  std::string filename{"atomics.h5"};

  std::vector<double> doubles = { 1.0, 32., 69., 420., -9.0 };
  std::vector<int>    ints    = { 0, -33, 88, 39, 123 };

  try { // Writing
    fire::Process p("test",filename,true);
    for (std::size_t i_entry{0}; i_entry < doubles.size(); i_entry++) {
      p.event().add("double", doubles.at(i_entry));
      p.event().add("int", ints.at(i_entry));
      bool positive{ints.at(i_entry) > 0};
      p.event().add("bool", positive);

      // needed for event counting, standing in for event header
      p.event().add("i_entry", i_entry);
      p.next();
    }
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    BOOST_REQUIRE(false);
  }

  try { // Reading
    fire::Process p("test",filename);
    std::size_t i_entry{0};
    do {
      auto d = p.event().get<double>("double");
      BOOST_CHECK(d == doubles.at(i_entry));

      auto i = p.event().get<int>("int");
      BOOST_CHECK(i == ints.at(i_entry));

      auto b = p.event().get<bool>("bool");
      BOOST_CHECK(b == (ints.at(i_entry) > 0));

      i_entry++;
    } while(p.next());
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    BOOST_REQUIRE(false);
  }
}

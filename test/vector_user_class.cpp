#include <boost/test/tools/interface.hpp>
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "fire/Process.hpp"
#include "fire/h5/DataSet.hpp"

class Hit {
  double energy_;
  int id_;
 private:
  friend class fire::h5::DataSet<Hit>;
  void attach(fire::h5::DataSet<Hit>& set) {
    set.attach("energy",energy_);
    set.attach("id",id_);
  }
 public:
  Hit() = default;
  Hit(double e, int id) : energy_{e}, id_{id} {}
  bool operator==(Hit const& other) const {
    return energy_ == other.energy_ and id_ == other.id_;
  }
};

class Cluster {
  int id_;
  std::vector<Hit> hits_;
 private:
  friend class fire::h5::DataSet<Cluster>;
  void attach(fire::h5::DataSet<Cluster>& set) {
    set.attach("id", id_);
    set.attach("hits",hits_);
  }
 public:
  Cluster() = default;
  Cluster(int id, std::vector<Hit> const& hits) : id_{id}, hits_{hits} {}
  bool operator==(Cluster const& other) const {
    if (id_ != other.id_) return false;
    if (hits_.size() != other.hits_.size()) return false;
    for (std::size_t i{0}; i < hits_.size(); i++) {
      if (not(hits_.at(i) == other.hits_.at(i))) return false;
    }
    return true;
  }
};

BOOST_AUTO_TEST_CASE(vector_user_class) {
  std::string filename{"vector_user_class.h5"};

  std::vector<
    std::vector<Hit>
    > all_hits = {
      {Hit(25.,1), Hit(32.,2), Hit(1234.,10)},
      {Hit(25.,1), Hit(32.,2), Hit(23.,10), Hit(321.,69)},
      {Hit(2.,1), Hit(31.,6)}
    };

  try { // Writing
    fire::Process p("test",filename,true);
    for (std::size_t i_entry{0}; i_entry < all_hits.size(); i_entry++) {
      p.event().add("hits", all_hits.at(i_entry));

      auto c = Cluster(i_entry, all_hits.at(i_entry));
      p.event().add("cluster", c);

      std::vector<Cluster> clusters;
      clusters.emplace_back(i_entry, all_hits.at(0));
      clusters.emplace_back(i_entry-1, all_hits.at(1));

      p.event().add("clusters", clusters);

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
      auto hits = p.event().get<std::vector<Hit>>("hits");
      BOOST_CHECK(hits == all_hits.at(i_entry));

      auto correct_c = Cluster(i_entry, all_hits.at(i_entry));
      auto c = p.event().get<Cluster>("cluster");
      BOOST_CHECK(c == correct_c);

      std::vector<Cluster> correct_clusters;
      correct_clusters.emplace_back(i_entry, all_hits.at(0));
      correct_clusters.emplace_back(i_entry-1, all_hits.at(1));
      auto clusters = p.event().get<std::vector<Cluster>>("clusters");
      BOOST_CHECK(clusters == correct_clusters);

      i_entry++;
    } while(p.next());
  } catch (std::exception const& e) {
    std::cout << e.what() << std::endl;
    BOOST_REQUIRE(false);
  }
}


#include "fire/Processor.h"
#include "Bench/Event/Hit.h"

#include <random>

namespace bench {

class Produce : public fire::Producer {
  /// the random number generator, unseeded so it produces the same results each time
  std::mt19937 rng;
  /// the distribution of sizes
  std::uniform_int_distribution<std::size_t> rand_size;
  /// the distribution of values in the doubles vector
  std::uniform_real_distribution<double> rand_value; 
 public:
  Produce(const fire::config::Parameters& ps)
    : fire::Producer(ps),
    rng{}, // this is where a seed for the RNG would be put
    rand_size{1, std::size_t(ps.get<int>("max_size",10))},
    rand_value{0.,1.}
  {}
  ~Produce() = default;
  void produce(fire::Event& event) final override {
    std::vector<double> rand_data;
    rand_data.resize(rand_size(rng));
    for (double& val : rand_data) val = rand_value(rng);
    event.add("rand_data", rand_data);
  }
};  // Produce

}

DECLARE_PROCESSOR_NS(bench,Produce);

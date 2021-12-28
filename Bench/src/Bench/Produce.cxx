
#include "Framework/EventProcessor.h"
#include "Bench/Event/Hit.h"

#include <random>

namespace bench {

class Produce : public framework::Producer {
  std::mt19937 rng;
  std::uniform_int_distribution<std::size_t> rand_size;
  std::uniform_real_distribution<double> rand_value;
 public:
  Produce(const std::string& name, framework::Process& p)
    : framework::Producer(name,p),
    rng{},
    rand_size{1,100},
    rand_value{0.,1.}
  {}
  ~Produce() = default;
  void produce(framework::Event& event) final override {
    std::vector<double> rand_data;
    rand_data.resize(rand_size(rng));
    for (double& val : rand_data) val = rand_value(rng);
    event.add("randdata", rand_data);
  }
};  // Produce

}

DECLARE_PRODUCER_NS(bench,Produce);

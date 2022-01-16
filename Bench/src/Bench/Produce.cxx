
#include "Bench/Event/Hit.h"

#ifdef USE_ROOT
#include "Framework/EventProcessor.h"
#else
#include "fire/Processor.h"
#endif

#include <random>

namespace bench {

#ifdef USE_ROOT
namespace fire = framework;
#endif

class Produce : public fire::Producer {
  /// the random number generator, unseeded so it produces the same results each time
  std::mt19937 rng;
  /// the distribution of sizes
  std::uniform_int_distribution<std::size_t> rand_size;
  /// the distribution of values in the Hits vector
  std::uniform_real_distribution<float> rand_float; 
  /// the distribution of integer values
  std::uniform_int_distribution<long int> rand_int;
 public:
#ifdef USE_ROOT
  Produce(const std::string& name, framework::Process& p)
    : framework::Producer(name,p),
#else
  Produce(const fire::config::Parameters& ps)
    : fire::Producer(ps),
#endif
    rng{}, // this is where a seed for the RNG would be put
    rand_size{1, 100},
    rand_float{0.,100.},
    rand_int{-100,100}
  {}
  ~Produce() = default;
  void produce(fire::Event& event) final override {
    std::vector<Hit> rand_data;
    rand_data.resize(rand_size(rng));
    for (Hit& val : rand_data) {
      val.setLayerID(rand_int(rng));
      val.setModuleID(rand_int(rng));
      val.setTrackID(rand_int(rng));
      val.setPdgID(rand_int(rng));
      val.setPosition(rand_float(rng),rand_float(rng),rand_float(rng));
      val.setEnergy(rand_float(rng));
      val.setTime(rand_float(rng));
      val.setMomentum(rand_float(rng),rand_float(rng),rand_float(rng));
    }
    event.add("randdata", rand_data);
  }
};  // Produce

}

#ifdef USE_ROOT
DECLARE_PRODUCER_NS(bench,Produce);
#else
DECLARE_PROCESSOR_NS(bench,Produce);
#endif

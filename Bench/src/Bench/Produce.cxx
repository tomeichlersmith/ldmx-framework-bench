
#include "Framework/EventProcessor.h"
#include "Bench/Event/Hit.h"

namespace bench {

class Produce : public framework::Producer {
 public:
  Produce(const std::string& name, framework::Process& p)
    : framework::Producer(name,p) {}
  ~Produce() = default;
  void produce(framework::Event& event) final override {
    Hit h;
    event.add("hit",h);
  }
};  // Produce

}

DECLARE_PRODUCER_NS(bench,Produce);


#include "fire/Processor.hpp"
#include "Bench/Event/Hit.h"

namespace bench {

class Produce : public fire::Producer {
 public:
  Produce(const std::string& name, fire::Process& p)
    : fire::Producer(name,p) {}
  ~Produce() = default;
  void produce(fire::Event& event) final override {
    Hit h;
    event.add("hit",h);
  }
};  // Produce

}

DECLARE_PROCESSOR_NS(bench,Produce);

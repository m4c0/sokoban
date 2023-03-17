#include "../casein/build.hpp"
#include "ecow.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  auto m = unit::create<mod>("sokoban");
  m->add_wsdep("casein", casein());

  auto a = unit::create<app>("sokoban");
  a->add_ref(m);
  return run_main(a, argc, argv);
}

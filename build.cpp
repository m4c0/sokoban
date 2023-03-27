#include "../casein/build.hpp"
#include "../quack/build.hpp"
#include "ecow.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  auto m = unit::create<mod>("sokoban");
  m->add_wsdep("casein", casein());
  m->add_wsdep("quack", quack());
  m->add_part("atlas");
  m->add_part("levels");
  m->add_part("game");

  auto a = unit::create<app>("sokoban");
  a->add_ref(m);
  return run_main(a, argc, argv);
}

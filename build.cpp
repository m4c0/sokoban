#include "ecow.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  auto a = unit::create<app>("sokoban");
  return run_main(a, argc, argv);
}

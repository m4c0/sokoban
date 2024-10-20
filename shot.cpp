#ifndef LECO_TARGET_IOS
#pragma leco app
#endif

import casein;
import game;

namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static void set_level(int dl) {
  auto ll = (sl::max_levels() + sl::current_level() + dl) % sl::max_levels();

  sl::load_level(sl::level(ll), ll);

  sr::set_updater([](auto *& i) -> void {
    sr::update_data(i, {}, {}, false, false);
  });
}

static void render_shots() {
}

struct init {
  init() {
    using namespace casein;
    handle(KEY_DOWN, K_LEFT, [] { set_level(-1); });
    handle(KEY_DOWN, K_RIGHT, [] { set_level(1); });
    handle(KEY_DOWN, K_ENTER, render_shots);

    sl::read_levels();
    set_level(0);
  }
} i;

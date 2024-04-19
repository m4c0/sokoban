#pragma leco app
export module maped;
import casein;
import game;

namespace sgg = sokoban::game_grid;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static unsigned l{2};

static void left() {
  l = l == 0 ? sl::max_levels() - 1 : l - 1;
  sgg::set_level(l);
  sr::render();
}
static void right() {
  l = l == (sl::max_levels() - 1) ? 0 : l + 1;
  sgg::set_level(l);
  sr::render();
}

static struct init {
  init() {
    sgg::set_level(l);

    using namespace casein;
    handle(KEY_DOWN, K_LEFT, &left);
    handle(KEY_DOWN, K_RIGHT, &right);
  }
} i;

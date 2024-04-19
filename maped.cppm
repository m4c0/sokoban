#pragma leco app
export module maped;
import casein;
import game;

namespace sgg = sokoban::game_grid;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static int g_lvl{0};
static void set_level(int l) {
  g_lvl = (sl::max_levels() + l) % sl::max_levels();
  sgg::set_level(g_lvl);
  sr::render();
}
static void prev_level() { set_level(g_lvl - 1); }
static void next_level() { set_level(g_lvl + 1); }
static void level_select() {
  using namespace casein;
  handle(KEY_DOWN, K_P, &prev_level);
  handle(KEY_DOWN, K_N, &next_level);
}

static struct init {
  init() {
    sgg::set_level(g_lvl);
    level_select();
  }
} i;

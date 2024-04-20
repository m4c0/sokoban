#pragma leco app
export module maped;
import casein;
import game;
import silog;

namespace sgg = sokoban::game_grid;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static int g_lvl{0};
static int g_cursor{0};

static auto &lw = sl::level_width;

static void set_level(int l) {
  g_lvl = (sl::max_levels() + l) % sl::max_levels();
  silog::log(silog::info, "Changing editor to level %d", g_lvl);
  sgg::set_level(g_lvl);
  sr::render();
}
static void prev_level() { set_level(g_lvl - 1); }
static void next_level() { set_level(g_lvl + 1); }

static void cursor_left() {
  if (g_cursor % lw == 0) {
    g_cursor += lw - 1;
  } else {
    g_cursor--;
  }
}
static void cursor_right() {
  if (g_cursor % lw == lw + 1) {
    g_cursor -= lw - 1;
  } else {
    g_cursor++;
  }
}

static void level_select();
static void edit_level() {
  using namespace casein;
  reset_k(KEY_DOWN);
  handle(KEY_DOWN, K_ESCAPE, &level_select);
  handle(KEY_DOWN, K_LEFT, &cursor_left);
  handle(KEY_DOWN, K_RIGHT, &cursor_right);
}
static void level_select() {
  using namespace casein;
  reset_k(KEY_DOWN);
  handle(KEY_DOWN, K_LEFT, &prev_level);
  handle(KEY_DOWN, K_RIGHT, &next_level);
  handle(KEY_DOWN, K_ENTER, &edit_level);
}

static struct init {
  init() {
    sgg::set_level(g_lvl);
    level_select();
  }
} i;

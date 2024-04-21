#pragma leco app
export module maped;
import casein;
import game;
import silog;
import quack;

namespace sgg = sokoban::game_grid;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static int g_lvl{0};
static int g_cursor{-1};

static auto &lh = sl::level_height;
static auto &lw = sl::level_width;

static struct : sr::rnd {
  const char *app_name() const noexcept override { return "maped"; }

  void update_data(quack::mapped_buffers all) override {
    rnd::update_data(all);

    if (g_cursor < 0)
      return;

    auto &c = all.colours[g_cursor];
    c.r *= 0.2;
    c.g *= 0.2;
    c.b *= 0.2;
    c.a = 1.0;
  }
} r;

static void set_level(int l) {
  g_lvl = (sl::max_levels() + l) % sl::max_levels();
  silog::log(silog::info, "Changing editor to level %d", g_lvl);
  sgg::set_level(g_lvl);
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
  if (g_cursor % lw == lw - 1) {
    g_cursor -= lw - 1;
  } else {
    g_cursor++;
  }
}
static void cursor_up() {
  g_cursor -= lw;
  if (g_cursor < 0)
    g_cursor += lw * lh;
}
static void cursor_down() {
  g_cursor = (g_cursor + lw) % (lw * lh);
}

static void level_select();
static void edit_level() {
  using namespace casein;
  reset_k(KEY_DOWN);
  handle(KEY_DOWN, K_ESCAPE, &level_select);
  handle(KEY_DOWN, K_LEFT, &cursor_left);
  handle(KEY_DOWN, K_RIGHT, &cursor_right);
  handle(KEY_DOWN, K_DOWN, &cursor_down);
  handle(KEY_DOWN, K_UP, &cursor_up);
  g_cursor = 0;
}
static void level_select() {
  using namespace casein;
  reset_k(KEY_DOWN);
  handle(KEY_DOWN, K_LEFT, &prev_level);
  handle(KEY_DOWN, K_RIGHT, &next_level);
  handle(KEY_DOWN, K_ENTER, &edit_level);
  g_cursor = -1;
}

static void refresh() { r.refresh_batch(); }
struct init {
  init() {
    sgg::set_level(g_lvl);
    level_select();

    using namespace casein;
    handle(TIMER, &refresh);
  }
} i;

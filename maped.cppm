#pragma leco app
export module maped;
import casein;
import game;
import silog;
import quack;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static char g_lvl_buf[1024];

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

  const char *gl = sl::level(g_lvl);
  char *nl = g_lvl_buf;
  for (; *gl; nl++, gl++)
    *nl = *gl;

  sg::set_level(g_lvl_buf);
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

// TODO: add support to player starting from a target
// Note: this requires support in the game itself. Currently, it crashes if
// player starts in a target.
static void clear_player() {
  switch (auto &pp = g_lvl_buf[sg::player_pos]) {
  case player:
    pp = empty;
    break;
  default:
    silog::log(silog::error, "invalid map state in old player pos: [%c]", pp);
    throw 0;
  }
}

static void update(char b) {
  g_lvl_buf[g_cursor] = b;
  sg::set_level(g_lvl_buf);
}
static void set_player() {
  switch (g_lvl_buf[g_cursor]) {
  case empty:
    clear_player();
    update(player);
    break;
  default:
    break;
  }
}

static void set_wall() {
  switch (g_lvl_buf[g_cursor]) {
  case empty:
  case outside:
    update(wall);
    break;
  default:
    break;
  }
}

static void set_void() {
  switch (g_lvl_buf[g_cursor]) {
  case outside:
  case wall:
    update(empty);
    break;
  case empty:
    update(outside);
    break;
  default:
    break;
  }
}

static void level_dump() {
  const char *l = g_lvl_buf;
  for (auto y = 0; y < sl::level_height; y++, l += sl::level_width) {
    silog::log(silog::info, "%.*s", sl::level_width, l);
  }
}

static void level_select();
static void edit_level() {
  using namespace casein;
  reset_k(KEY_DOWN);
  handle(KEY_DOWN, K_ESCAPE, &level_select);
  handle(KEY_DOWN, K_ENTER, &level_dump);
  handle(KEY_DOWN, K_LEFT, &cursor_left);
  handle(KEY_DOWN, K_RIGHT, &cursor_right);
  handle(KEY_DOWN, K_DOWN, &cursor_down);
  handle(KEY_DOWN, K_UP, &cursor_up);
  handle(KEY_DOWN, K_P, &set_player);
  handle(KEY_DOWN, K_W, &set_wall);
  handle(KEY_DOWN, K_SPACE, &set_void);
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
    set_level(g_lvl);
    level_select();

    using namespace casein;
    handle(TIMER, &refresh);
  }
} i;

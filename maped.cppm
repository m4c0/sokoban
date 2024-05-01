#pragma leco app
export module maped;
import casein;
import fork;
import game;
import jute;
import quack;
import silog;
import yoyo;

// TODO: flood fill empty/outside
// TODO: maybe support for map without a player?
// Note: these requires support in the game itself. Currently, it crashes if
// player starts in a target.

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;
using namespace sokoban::enums;

static char g_lvl_buf[1024];

static int g_cursor{-1};
static void (*g_pen)();

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

static void set_level(int dl) {
  auto ll = (sl::max_levels() + sl::current_level() + dl) % sl::max_levels();
  silog::log(silog::info, "Changing editor to level %d", ll);

  char *nl = g_lvl_buf;
  for (auto c : sl::level(ll))
    *nl++ = c;

  sl::load_level(g_lvl_buf, ll);
}
static void prev_level() { set_level(-1); }
static void next_level() { set_level(+1); }

static void new_level() {
  silog::log(silog::info, "New level");

  for (auto &c : g_lvl_buf)
    c = outside;

  sl::load_level(g_lvl_buf, sl::max_levels());
}

static void cursor_left() {
  if (g_cursor % lw == 0) {
    g_cursor += lw - 1;
  } else {
    g_cursor--;
  }
  if (g_pen)
    g_pen();
}
static void cursor_right() {
  if (g_cursor % lw == lw - 1) {
    g_cursor -= lw - 1;
  } else {
    g_cursor++;
  }
  if (g_pen)
    g_pen();
}
static void cursor_up() {
  g_cursor -= lw;
  if (g_cursor < 0)
    g_cursor += lw * lh;
  if (g_pen)
    g_pen();
}
static void cursor_down() {
  g_cursor = (g_cursor + lw) % (lw * lh);
  if (g_pen)
    g_pen();
}

static void clear_player() {
  if (sg::player_pos >= sl::level_width * sl::level_height)
    return;

  switch (auto &pp = g_lvl_buf[sg::player_pos]) {
  case player:
    pp = empty;
    break;
  case player_target:
    pp = target;
    break;
  default:
    silog::log(silog::error, "invalid map state in old player pos: [%c]", pp);
    throw 0;
  }
}

static void move_lvl_left() {
  const auto count = sl::level_width * sl::level_height;
  for (auto i = 0; i < count - 1; i++) {
    g_lvl_buf[i] = g_lvl_buf[i + 1];
  }
  g_lvl_buf[count - 1] = outside;
  sl::load_level(g_lvl_buf, sl::current_level());
}
static void move_lvl_right() {
  const auto count = sl::level_width * sl::level_height;
  for (auto i = count; i > 0; i--) {
    g_lvl_buf[i] = g_lvl_buf[i - 1];
  }
  g_lvl_buf[0] = outside;
  sl::load_level(g_lvl_buf, sl::current_level());
}
static void move_lvl_up() {
  const auto count = sl::level_width * sl::level_height;
  for (auto i = 0; i < count - sl::level_width; i++) {
    g_lvl_buf[i] = g_lvl_buf[i + sl::level_width];
  }
  for (auto i = 0; i < sl::level_width; i++)
    g_lvl_buf[count - i] = outside;
  sl::load_level(g_lvl_buf, sl::current_level());
}
static void move_lvl_down() {
  const auto count = sl::level_width * sl::level_height;
  for (auto i = count; i >= sl::level_width; i--) {
    g_lvl_buf[i] = g_lvl_buf[i - sl::level_width];
  }
  for (auto i = 0; i < sl::level_width; i++)
    g_lvl_buf[i] = outside;
  sl::load_level(g_lvl_buf, sl::current_level());
}

static void update(char b) {
  g_lvl_buf[g_cursor] = b;
  sl::load_level(g_lvl_buf, sl::current_level());
}
static void set_player() {
  switch (g_lvl_buf[g_cursor]) {
  case empty:
    clear_player();
    update(player);
    break;
  case target:
    clear_player();
    update(player_target);
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
  g_pen = set_wall;
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
  g_pen = set_void;
}

static void set_box() {
  switch (g_lvl_buf[g_cursor]) {
  case outside:
  case empty:
    update(box);
    break;
  case box:
    update(empty);
    break;
  case target:
    update(target_box);
    break;
  case target_box:
    update(target);
    break;
  default:
    break;
  }
}
static void set_target() {
  switch (g_lvl_buf[g_cursor]) {
  case outside:
  case empty:
    update(target);
    break;
  case target:
    update(empty);
    break;
  case box:
    update(target_box);
    break;
  case target_box:
    update(box);
    break;
  default:
    break;
  }
}

static void reset_pen() { g_pen = {}; }

static mno::req<void> store_level(int l, yoyo::writer *w) {
  const auto lw = sl::level_width;
  const auto lh = sl::level_height;
  jute::view lvl = (l == sl::current_level()) ? g_lvl_buf : sl::level(l);
  return w->write_u32(l).fmap([&] { return w->write(lvl.data(), lw * lh); });
}
static mno::req<void> store_levels(int l, yoyo::writer *w) {
  if (l >= sl::max_levels() && l != sl::current_level()) {
    return {};
  }
  return frk::push('LEVL', w, [&](auto) { return store_level(l, w); })
      .fmap([&] { return store_levels(l + 1, w); });
}
static void level_dump() {
  const char *l = g_lvl_buf;
  for (auto y = 0; y < sl::level_height; y++, l += sl::level_width) {
    silog::log(silog::info, "%.*s", sl::level_width, l);
  }

  yoyo::file_writer w{"levels.dat"};
  frk::push('SKBN', &w, [&](auto w) {
    return store_levels(0, w);
  }).take([](auto err) {
    silog::log(silog::error, "failed to write levels: %s", err);
  });
}

static void level_select();
static void edit_level() {
  using namespace casein;
  reset_k(KEY_DOWN);
  reset_k(KEY_UP);

  handle(KEY_DOWN, K_ESCAPE, &level_select);
  handle(KEY_DOWN, K_ENTER, &level_dump);
  handle(KEY_DOWN, K_LEFT, &cursor_left);
  handle(KEY_DOWN, K_RIGHT, &cursor_right);
  handle(KEY_DOWN, K_DOWN, &cursor_down);
  handle(KEY_DOWN, K_UP, &cursor_up);
  handle(KEY_DOWN, K_P, &set_player);
  handle(KEY_DOWN, K_B, &set_box);
  handle(KEY_DOWN, K_T, &set_target);
  handle(KEY_DOWN, K_W, &set_wall);
  handle(KEY_DOWN, K_SPACE, &set_void);

  handle(KEY_DOWN, K_H, &move_lvl_left);
  handle(KEY_DOWN, K_J, &move_lvl_down);
  handle(KEY_DOWN, K_K, &move_lvl_up);
  handle(KEY_DOWN, K_L, &move_lvl_right);

  handle(KEY_UP, K_W, &reset_pen);
  handle(KEY_UP, K_SPACE, &reset_pen);
  g_cursor = 0;
}
static void level_select() {
  using namespace casein;
  reset_k(KEY_DOWN);
  reset_k(KEY_UP);
  handle(KEY_DOWN, K_LEFT, &prev_level);
  handle(KEY_DOWN, K_RIGHT, &next_level);
  handle(KEY_DOWN, K_ENTER, &edit_level);
  handle(KEY_DOWN, K_N, &new_level);
  g_cursor = -1;
}

static void refresh() { r.refresh_batch(); }
struct init {
  init() {
    set_level(0);
    level_select();

    using namespace casein;
    handle(TIMER, &refresh);
  }
} i;

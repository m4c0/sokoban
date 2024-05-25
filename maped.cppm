#pragma leco app
export module maped;
import casein;
import fork;
import game;
import jute;
import quack;
import silog;
import sprites;
import yoyo;

// TODO: maybe support for map without a player?

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;
using namespace spr;

static char g_lvl_buf[sg::max_quads];

static int g_cursor{-1};
static void (*g_pen)();

static auto &lh = sl::level_height;
static auto &lw = sl::level_width;

static void read_levels() {
  yoyo::file_reader::open("levels.dat")
      .fmap([](auto &&r) { return sl::read_levels(&r); })
      .take([](auto msg) {
        silog::log(silog::error, "failed to load levels: %s", msg);
      });
}

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

static void set_empty() {
  switch (g_lvl_buf[g_cursor]) {
  case outside:
    update(empty);
    break;
  default:
    break;
  }
  g_pen = set_empty;
}

static void set_outside() {
  update(outside);
  g_pen = set_outside;
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
  g_pen = set_target;
}

static void fill(int p) {
  if (p < 0 || p >= sl::level_quad_count())
    return;

  auto &c = g_lvl_buf[p];
  if (c != outside)
    return;

  c = empty;
  fill(p - 1);
  fill(p + 1);
  fill(p - sl::level_width);
  fill(p + sl::level_width);
}
static void fill() {
  fill(g_cursor);
  sl::load_level(g_lvl_buf, sl::current_level());
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
  yoyo::file_writer::open("levels.dat")
      .fmap([](auto &&w) {
        return frk::push('SKBN', &w,
                         [&](auto w) { return store_levels(0, w); });
      })
      .map([] { silog::log(silog::info, "Levels persisted"); })
      .take([](auto err) {
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
  handle(KEY_DOWN, K_X, &set_outside);
  handle(KEY_DOWN, K_Z, &fill);
  handle(KEY_DOWN, K_SPACE, &set_empty);

  handle(KEY_DOWN, K_H, &move_lvl_left);
  handle(KEY_DOWN, K_J, &move_lvl_down);
  handle(KEY_DOWN, K_K, &move_lvl_up);
  handle(KEY_DOWN, K_L, &move_lvl_right);

  handle(KEY_UP, K_W, &reset_pen);
  handle(KEY_UP, K_X, &reset_pen);
  handle(KEY_UP, K_T, &reset_pen);
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

  read_levels();
  set_level(0);
}

static unsigned validate_level(quack::mapped_buffers &all) {
  auto players = false;
  auto boxes = 0U;
  auto targets = 0U;

  for (auto c : g_lvl_buf) {
    switch (c) {
    case player:
      players = true;
      break;
    case box:
      boxes++;
      break;
    case target:
      targets++;
      break;
    case target_box:
      boxes++;
      targets++;
      break;
    default:
      break;
    }
  }

  auto count = 0U;
  if (!players)
    count += spr::blit::block(all, count, 1, player);
  if (boxes > targets || targets == 0)
    count += spr::blit::block(all, count, 1, target);
  if (boxes < targets)
    count += spr::blit::block(all, count, 1, box);

  return count;
}

static void refresh() {
  using namespace quack::donald;
  data([](auto all) -> unsigned {
    auto all2 = all;
    auto count = sr::update_data(all2);
    count += validate_level(all2);

    if (g_cursor < 0)
      return count;

    auto &c = all.colours[g_cursor];
    c.r *= 0.2;
    c.g *= 0.2;
    c.b *= 0.2;
    c.a = 1.0;
    return count;
  });
}
struct init {
  init() {
    level_select();

    using namespace casein;
    handle(TIMER, &refresh);

    using namespace quack::donald;
    app_name("maped");
  }
} i;

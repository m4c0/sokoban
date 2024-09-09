module sokoban;
import buoy;

enum move_type { push, walk, none, push2tgt };

static void update_data(quack::instance *& all) {
  sr::update_data(all);
}

static void set_level(unsigned idx) {
  sl::load_level(idx);
  quack::donald::data(update_data);

  auto max = save::read().max_level;
  if (idx > max)
    max = idx;
  save::write({
      .cur_level = idx,
      .max_level = max,
  });
}

static bool is_done() {
  for (auto i = 0; i < sl::level_quad_count(); i++)
    if (sg::grid[i] == target)
      return false;

  return true;
}

static void set_box(unsigned p) {
  sg::grid[p] = (sg::grid[p] == target) ? target_box : box;
}
static void clear_box(unsigned p) {
  sg::grid[p] = (sg::grid[p] == target_box) ? target : empty;
}

static auto move_type(unsigned delta) noexcept {
  auto next = sg::player_pos + delta;

  switch (sg::grid[next]) {
  case box:
  case target_box:
    switch (sg::grid[next + delta]) {
    case empty:
      return push;
    case target:
      return push2tgt;
    default:
      return none;
    }
  case outside:
  case wall:
    return none;
  case empty:
  case target:
  case player:
  case player_target:
    return walk;
  }
}

static void move(unsigned p) {
  switch (auto mt = move_type(p)) {
  case none:
    play_tone(150);
    return;
  case push:
  case push2tgt:
    sg::player_pos += p;
    set_box(sg::player_pos + p);
    clear_box(sg::player_pos);
    if (is_done()) {
      set_level(sl::current_level() + 1);
      play_tone(50);
    } else if (mt == push2tgt) {
      play_tone(100);
    } else {
      play_tone(200);
    }
    break;
  case walk:
    sg::player_pos += p;
    break;
  }
  quack::donald::data(update_data);
}

static void reset_level() { set_level(sl::current_level()); }

static void down() { move(sl::level_width); }
static void up() { move(-sl::level_width); }
static void left() { move(-1); }
static void right() { move(1); }

void setup_game() {
  using namespace casein;
  handle(GESTURE, G_SWIPE_UP, &up);
  handle(GESTURE, G_SWIPE_DOWN, &down);
  handle(GESTURE, G_SWIPE_LEFT, &left);
  handle(GESTURE, G_SWIPE_RIGHT, &right);
  handle(GESTURE, G_SHAKE, &reset_level);

  handle(KEY_DOWN, K_UP, &up);
  handle(KEY_DOWN, K_DOWN, &down);
  handle(KEY_DOWN, K_LEFT, &left);
  handle(KEY_DOWN, K_RIGHT, &right);

  handle(KEY_DOWN, K_W, &up);
  handle(KEY_DOWN, K_S, &down);
  handle(KEY_DOWN, K_A, &left);
  handle(KEY_DOWN, K_D, &right);

  handle(KEY_DOWN, K_K, &up);
  handle(KEY_DOWN, K_J, &down);
  handle(KEY_DOWN, K_H, &left);
  handle(KEY_DOWN, K_L, &right);

  handle(KEY_DOWN, K_SPACE, &reset_level);
  handle(KEY_DOWN, K_ESCAPE, &open_menu);

  quack::donald::data(update_data);
}

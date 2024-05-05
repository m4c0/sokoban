module sokoban;

enum move_type { push, walk, none, push2tgt };

static void set_level(unsigned idx) {
  sl::load_level(idx);
  r.refresh_batch();
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
    audio.play(150);
    return;
  case push:
  case push2tgt:
    sg::player_pos += p;
    set_box(sg::player_pos + p);
    clear_box(sg::player_pos);
    if (is_done()) {
      set_level(sl::current_level() + 1);
      audio.play(50);
    } else if (mt == push2tgt) {
      audio.play(100);
    } else {
      audio.play(200);
    }
    break;
  case walk:
    sg::player_pos += p;
    break;
  }
  r.refresh_batch();
}

static void reset_level() { set_level(sl::current_level()); }

static void down() { move(sl::level_width); }
static void up() { move(-sl::level_width); }
static void left() { move(-1); }
static void right() { move(1); }

static void flip_fullscreen() {
  casein::set_fullscreen(!casein::is_fullscreen());
}

void setup_game(int l) {
  sl::load_level(l);

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
  handle(KEY_DOWN, K_SPACE, &reset_level);
  // TODO: add F11 to casein
  handle(KEY_DOWN, K_F, &flip_fullscreen);
}

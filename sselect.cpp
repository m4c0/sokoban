module sokoban;
import sprites;

static unsigned max_level;

static void update_data(quack::instance *& all) {
  static constexpr const float w = 10.0f;
  static constexpr const float h = 6.0f;

  auto sx = 1.5f * ((sl::current_level() % 10) - 5.0f + 0.5f) / 0.8f;
  auto sy = 1.5f * ((sl::current_level() / 10) - 3.0f + 0.5f) / 0.8f;
  sr::update_data(all, { w, h }, { sx, sy, 0.8f, 0.6f });

  for (auto i = 0; i <= max_level; i++) {
    float x = 6.0f + (i % 10) * 2.24f;
    float y = 6.0f + (i / 10) * 2.24f;
    spr::blit::number(all, i + 1, x, y);
  }
}

static void set_level(int l) {
  if (l < 0) return;
  if (l > max_level) return;

  sl::load_level(l);

  sokoban::renderer::set_updater(update_data);
}
static void change_level(int d) {
  int l = sl::current_level() + d;
  set_level(l);
}

static void left() { change_level(-1); }
static void right() { change_level(1); }
static void down() { change_level(10); }
static void up() { change_level(-10); }

static void mouse_move() {
  auto p = sr::mouse_pos();
  p = (p - 5.0f) / 2.24f;

  int x = p.x;
  int y = p.y;
  if (x < 0 || x >= 10 || y < 0 || y >= 5) return;
  set_level(y * 10 + x);
}

void open_level_select() {
  max_level = save::read().max_level;

  using namespace casein;
  reset_casein();

  handle(KEY_DOWN, K_ESCAPE, open_menu);
  handle(KEY_DOWN, K_ENTER, open_menu);

  handle(KEY_DOWN, K_LEFT, left);
  handle(KEY_DOWN, K_RIGHT, right);
  handle(KEY_DOWN, K_DOWN, down);
  handle(KEY_DOWN, K_UP, up);

  handle(MOUSE_MOVE, mouse_move);
  handle(TOUCH_MOVE, mouse_move);
  handle(TOUCH_DOWN, mouse_move);
  handle(TOUCH_UP, mouse_move);

  sokoban::renderer::set_updater(update_data);
}

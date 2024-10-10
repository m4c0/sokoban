module sokoban;
import sprites;

static unsigned max_level;

static void update_data(quack::instance *& all) {
  static constexpr const float w = 8.0f;
  static constexpr const float h = 4.5f;

  dotz::vec2 rp { sl::level_width * 0.5f - w, sl::level_height * 0.5f - h };

  sr::update_data(all, { w, h });

  for (auto i = 0; i <= max_level; i++) {
    float x = rp.x + (i % 10) * 2.0f;
    float y = rp.y + (i / 10) * 2.0f;
    if (i == sl::current_level()) {
      spr::blit::selection(all, x - 1.0f, y - 0.5f, 2.0f, 1.75f);
    }
    spr::blit::number(all, i + 1, x, y);
  }
}

static void change_level(int d) {
  int l = sl::current_level() + d;
  if (l < 0) return;
  if (l > max_level) return;

  sl::load_level(l);

  sokoban::renderer::set_updater(update_data);
}

static void left() { change_level(-1); }
static void right() { change_level(1); }
static void down() { change_level(10); }
static void up() { change_level(-10); }

void open_level_select() {
  max_level = save::read().max_level;

  using namespace casein;
  reset_g(GESTURE);
  reset_k(KEY_DOWN);

  handle(KEY_DOWN, K_ESCAPE, open_menu);
  handle(KEY_DOWN, K_ENTER, open_menu);

  handle(KEY_DOWN, K_LEFT, left);
  handle(KEY_DOWN, K_RIGHT, right);
  handle(KEY_DOWN, K_DOWN, down);
  handle(KEY_DOWN, K_UP, up);

  sokoban::renderer::set_updater(update_data);
}

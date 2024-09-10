module sokoban;
import sprites;

static unsigned max_level;

static void update_data(quack::instance *& all) {
  static constexpr const dotz::vec4 cur_level_bg{0, 0, 0, 1};

  dotz::vec2 rp{};
  dotz::vec2 rs{};
  ui::menu_bg(all, 17, 13, rp, rs);

  for (auto i = 0; i <= max_level; i++) {
    float x = rp.x + (i % 10) * 1.5f;
    float y = rp.y + (i / 10) * 1.5f;
    if (i == sl::current_level()) {
      ui::bg(all, 1.5, 1, cur_level_bg);
      all[-1].position.x = x - 0.75f;
      all[-1].position.y = y;
    }
    spr::blit::number(all, i + 1, x, y);
  }
}

static void change_level(int d) {
  int l = sl::current_level() + d;
  if (l < 0)
    return;
  if (l > max_level)
    return;

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

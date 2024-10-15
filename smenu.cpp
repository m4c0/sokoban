module sokoban;
import casein;
import sprites;

static unsigned g_sel {};

static constexpr const float w = 6.0f;
static constexpr const float h = 2.5f;
static constexpr const dotz::vec2 half_sz { w, h };

static void update_data(quack::instance *& all) {
  dotz::vec2 rp { sl::level_width, sl::level_height };
  rp = rp * 0.5f - half_sz;
  auto rr = rp.x + w * 2.0f;

  float sy = static_cast<float>(g_sel) / 0.8f - 1.5f / 0.8f;
  sr::update_data(all, { w, h }, { 0.0f, sy, w, 0.5f });

  auto au = is_audio_enabled();
  auto fs = casein::fullscreen;

  rp.x -= 0.5;
  rp.y -= 0.2;
  spr::blit::level(all, rp.x, rp.y);
  spr::blit::number(all, sl::current_level() + 1, rr, rp.y);
  spr::blit::sound(all, rp.x, rp.y + 1.5f);
  spr::blit::boolean(all, au, rr - 1.0, rp.y + 1.5f);
  spr::blit::fullscreen(all, rp.x, rp.y + 3.0f);
  spr::blit::boolean(all, fs, rr - 1.0, rp.y + 3.0f);
  spr::blit::restart(all, rp.x, rp.y + 4.5f);
}

static void toggle_audio() {
  enable_audio(!is_audio_enabled());
  sokoban::renderer::set_updater(update_data);
}
static void toggle_fullscreen() {
  casein::fullscreen = !casein::fullscreen;
  casein::interrupt(casein::IRQ_FULLSCREEN);
  sokoban::renderer::set_updater(update_data);
}

static void restart_level() {
  sl::load_level(sl::current_level());
  sokoban::renderer::set_updater(update_data);
  setup_game();
}

static void sel_down() {
  g_sel = (g_sel + 1) % 4;
  sokoban::renderer::set_updater(update_data);
}
static void sel_up() {
  g_sel = (4 + g_sel - 1) % 4;
  sokoban::renderer::set_updater(update_data);
}
static void sel_activate() {
  switch (g_sel) {
    case 0: open_level_select(); break;
    case 1: toggle_audio(); break;
    case 2: toggle_fullscreen(); break;
    case 3: restart_level(); break;
  }
}

static auto mouse_sel() {
  auto p = sr::mouse_pos();

  dotz::vec2 rp { sl::level_width, sl::level_height };
  p = p - (rp * 0.5 - half_sz);

  if (p.x < -3 || p.x > w * 2.0 + 3) return -1;

  int y = dotz::floor((p.y + 0.4f) / 1.5f);
  if (y < 0 || y > 3) return -1;
  return y;
}
static void mouse_move() {
  auto y = mouse_sel();
  if (y >= 0) g_sel = y;
  sokoban::renderer::set_updater(update_data);
}
static void mouse_up() {
  if (mouse_sel() == g_sel) sel_activate();
}

void open_menu() {
  using namespace casein;
  reset_casein();

  handle(KEY_DOWN, K_ESCAPE, &setup_game);
  handle(KEY_DOWN, K_UP, &sel_up);
  handle(KEY_DOWN, K_DOWN, &sel_down);
  handle(KEY_DOWN, K_ENTER, &sel_activate);

  handle(MOUSE_MOVE, mouse_move);
  handle(MOUSE_UP, mouse_up);

  handle(TOUCH_MOVE, mouse_move);
  handle(TOUCH_DOWN, mouse_move);
  handle(TOUCH_UP, mouse_up);

  g_sel = 0;
  sokoban::renderer::set_updater(update_data);
}

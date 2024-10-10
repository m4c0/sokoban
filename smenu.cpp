module sokoban;
import casein;
import sprites;

static unsigned g_sel{};

static void update_data(quack::instance *& all) {
  static constexpr const float w = 5.0f;
  static constexpr const float h = 2.0f;

  dotz::vec2 rp { sl::level_width * 0.5f - w, sl::level_height * 0.5f - h };
  auto rr = rp.x + w * 2.0f;

  sr::update_data(all, { w, h });
  ui::bg(all, 16, 1.5);

  auto &s = all[-1].position;
  s.y = rp.y - 1.0 + g_sel * 1.5f;

  auto au = is_audio_enabled();
  auto fs = casein::fullscreen;

  rp.x -= 0.5;
  rp.y -= 0.7;
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

void open_menu() {
  using namespace casein;
  reset_g(GESTURE);
  reset_k(KEY_DOWN);

  handle(KEY_DOWN, K_ESCAPE, &setup_game);
  handle(KEY_DOWN, K_UP, &sel_up);
  handle(KEY_DOWN, K_DOWN, &sel_down);
  handle(KEY_DOWN, K_ENTER, &sel_activate);

  g_sel = 0;
  sokoban::renderer::set_updater(update_data);
}

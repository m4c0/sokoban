module sokoban;
import casein;
import sprites;

static unsigned g_sel{};

static void update_data(quack::instance *& all) {
  static constexpr const dotz::vec4 sel_bg{0.1f, 0.4f, 0.3f, 0.7f};

  dotz::vec2 rp{};
  dotz::vec2 rs{};
  ui::menu_bg(all, 10, 4, rp, rs);

  auto rr = rp.x + rs.x;

  ui::bg(all, 16, 1.5, sel_bg);
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

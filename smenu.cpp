module sokoban;
import casein;
import sprites;

static unsigned g_sel{};

static quack::rect shrink(quack::rect r) {
  r.x += 2.0;
  r.y += 2.0;
  r.w -= 2.0;
  r.h -= 2.0;
  return r;
}

static unsigned update_data(quack::mapped_buffers all) {
  static constexpr const quack::colour sel_bg{0.1, 0.4, 0.3, 0.7};

  auto count = ui::menu_bg(all, 18, 10);

  auto r = shrink(all.positions[-1]);
  auto rr = r.x + r.w - 2;

  count += ui::bg(all, 16, 1.5, sel_bg);
  auto &s = all.positions[-1];
  s.y = r.y - 0.3f + g_sel * 1.5f;

  auto au = is_audio_enabled();
  auto fs = casein::is_fullscreen();

  count += spr::blit::level(all, r.x, r.y);
  count += spr::blit::number(all, sl::current_level() + 1, rr, r.y);
  count += spr::blit::sound(all, r.x, r.y + 1.5f);
  count += spr::blit::boolean(all, au, rr - 1.0, r.y + 1.5f);
  count += spr::blit::fullscreen(all, r.x, r.y + 3.0f);
  count += spr::blit::boolean(all, fs, rr - 1.0, r.y + 3.0f);

  return count;
}

static void toggle_audio() {
  enable_audio(!is_audio_enabled());
  quack::donald::data(update_data);
}
static void toggle_fullscreen() {
  casein::set_fullscreen(!casein::is_fullscreen());
  quack::donald::data(update_data);
}

static void sel_down() {
  g_sel = (g_sel + 1) % 3;
  quack::donald::data(update_data);
}
static void sel_up() {
  g_sel = (3 + g_sel - 1) % 3;
  quack::donald::data(update_data);
}
static void sel_activate() {
  switch (g_sel) {
  case 0:
    break;
  case 1:
    toggle_audio();
    break;
  case 2:
    toggle_fullscreen();
    break;
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
  quack::donald::data(update_data);
}

module sokoban;
import sprites;

static unsigned max_level;

static unsigned update_data(quack::mapped_buffers all) {
  static constexpr const quack::colour cur_level_bg{0, 0, 0, 1};

  quack::rect r{};
  auto count = ui::menu_bg(all, 17, 13, r);

  for (auto i = 0; i <= max_level; i++) {
    float x = r.x + (i % 10) * 1.5f;
    float y = r.y + (i / 10) * 1.5f;
    if (i == sl::current_level()) {
      count += ui::bg(all, 1.5, 1, cur_level_bg);
      all.positions[-1].x = x - 0.75f;
      all.positions[-1].y = y;
    }
    count += spr::blit::number(all, i + 1, x, y);
  }

  return count;
}

void open_level_select() {
  max_level = save::read().max_level;

  using namespace casein;
  reset_g(GESTURE);
  reset_k(KEY_DOWN);

  handle(KEY_DOWN, K_ESCAPE, open_menu);

  quack::donald::data(update_data);
}

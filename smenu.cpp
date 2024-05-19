module sokoban;
import sprites;

static unsigned dim(quack::mapped_buffers &all) {
  auto &[c, m, p, u] = all;
  float w = sl::level_width * 2.0;
  float h = sl::level_height * 2.0;

  *p++ = {{-w, -h}, {w * 2.0f, h * 2.0f}};
  *u++ = {};
  *c++ = {0, 0, 0, 0.9};
  *m++ = {1, 1, 1, 1};
  return 1;
}

static unsigned update_data(quack::mapped_buffers all) {
  auto count = sr::update_data(all);

  count += dim(all);
  count += spr::blit::level(all, 2, 2);
  count += spr::blit::number(all, sl::current_level(), 5, 2);

  return count;
}

void open_menu() {
  using namespace casein;
  reset_g(GESTURE);
  reset_k(KEY_DOWN);

  handle(KEY_DOWN, K_ESCAPE, &setup_game);

  quack::donald::data(update_data);
}

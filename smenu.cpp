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

static unsigned bg(quack::mapped_buffers &all) {
  auto &[c, m, p, u] = all;
  float w = sl::level_width * 0.5;
  float h = sl::level_height * 0.5;
  float x = sl::level_width - w;
  float y = sl::level_height - h;

  *p++ = {{x * 0.5f, y * 0.5f}, {w, h}};
  *u++ = {};
  *c++ = {0.1, 0.2, 0.3, 0.7};
  *m++ = {1, 1, 1, 1};
  return 1;
}

static quack::rect shrink(quack::rect r) {
  r.x++;
  r.y++;
  r.w--;
  r.h--;
  return r;
}

static unsigned update_data(quack::mapped_buffers all) {
  auto count = sr::update_data(all);

  count += dim(all);
  count += bg(all);

  auto rect = shrink(all.positions[-1]);
  auto rect_r = rect.x + rect.w;

  count += spr::blit::level(all, rect.x, rect.y);
  count += spr::blit::number(all, sl::current_level(), rect_r - 1.5, rect.y);

  return count;
}

void open_menu() {
  using namespace casein;
  reset_g(GESTURE);
  reset_k(KEY_DOWN);

  handle(KEY_DOWN, K_ESCAPE, &setup_game);

  quack::donald::data(update_data);
}

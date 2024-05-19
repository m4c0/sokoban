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

static unsigned level_opt(quack::mapped_buffers all, quack::rect r,
                          bool hover) {
  float mid = r.x + r.w * 0.5;

  unsigned count{};
  if (hover) {
    auto &[c, m, p, u] = all;
    *p++ = {{mid - 3.f, r.y}, {3.f + 1.5f, 1.0f}};
    *u++ = {};
    *c++ = {0.1, 0.5, 0.05, 0.6};
    *m++ = {1, 1, 1, 1};
    count++;
  }

  count += spr::blit::level(all, mid - 2.5f, r.y);
  count += spr::blit::number(all, sl::current_level(), mid + 0.5f, r.y);
  return count;
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
  auto bg_rect = shrink(all.positions[-1]);

  count += level_opt(all, bg_rect, true);

  return count;
}

void open_menu() {
  using namespace casein;
  reset_g(GESTURE);
  reset_k(KEY_DOWN);

  handle(KEY_DOWN, K_ESCAPE, &setup_game);

  quack::donald::data(update_data);
}

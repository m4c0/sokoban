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

static unsigned bg(quack::mapped_buffers &all, float w, float h) {
  auto &[c, m, p, u] = all;
  float x = sl::level_width - w;
  float y = sl::level_height - h;

  *p++ = {{x * 0.5f, y * 0.5f}, {w, h}};
  *u++ = {};
  *c++ = {0.1, 0.2, 0.3, 0.7};
  *m++ = {1, 1, 1, 1};
  return 1;
}

static quack::rect shrink(quack::rect r) {
  r.x += 2.0;
  r.y += 2.0;
  r.w -= 2.0;
  r.h -= 2.0;
  return r;
}

static unsigned update_data(quack::mapped_buffers all) {
  auto count = sr::update_data(all);

  count += dim(all);
  count += bg(all, 18, 10);

  auto r = shrink(all.positions[-1]);
  auto rr = r.x + r.w - 2;

  count += spr::blit::level(all, r.x, r.y);
  count += spr::blit::number(all, sl::current_level(), rr, r.y);
  count += spr::blit::sound(all, r.x, r.y + 1.5f);
  count += spr::blit::boolean(all, true, rr - 1.0, r.y + 1.5f);
  count += spr::blit::fullscreen(all, r.x, r.y + 3.0f);
  count += spr::blit::boolean(all, false, rr - 1.0, r.y + 3.0f);

  return count;
}

void open_menu() {
  using namespace casein;
  reset_g(GESTURE);
  reset_k(KEY_DOWN);

  handle(KEY_DOWN, K_ESCAPE, &setup_game);

  quack::donald::data(update_data);
}

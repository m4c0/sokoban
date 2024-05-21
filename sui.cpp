module sokoban;

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

unsigned ui::bg(quack::mapped_buffers &all, float w, float h,
                quack::colour cl) {
  auto &[c, m, p, u] = all;
  float x = sl::level_width - w;
  float y = sl::level_height - h;

  *p++ = {{x * 0.5f, y * 0.5f}, {w, h}};
  *u++ = {};
  *c++ = cl;
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

unsigned ui::menu_bg(quack::mapped_buffers &all, float w, float h,
                     quack::rect &r) {
  static constexpr const quack::colour diag_bg{0.1, 0.2, 0.3, 0.7};

  auto count = sr::update_data(all);

  count += dim(all);
  count += bg(all, w, h, diag_bg);

  r = shrink(all.positions[-1]);
  return count;
}

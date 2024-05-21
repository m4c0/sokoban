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

static unsigned bg(quack::mapped_buffers &all, float w, float h,
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

static unsigned update_data(quack::mapped_buffers all) {
  static constexpr const quack::colour diag_bg{0.1, 0.2, 0.3, 0.7};

  auto count = sr::update_data(all);

  count += dim(all);
  count += bg(all, 18, 10, diag_bg);

  return count;
}

void open_level_select() {
  using namespace casein;
  reset_g(GESTURE);
  reset_k(KEY_DOWN);

  quack::donald::data(update_data);
}

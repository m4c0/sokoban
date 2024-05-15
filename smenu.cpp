module sokoban;

static unsigned update_data(quack::mapped_buffers all) {
  auto count = sr::update_data(all);

  auto [c, m, p, u] = all;

  float w = sl::level_width * 2.0;
  float h = sl::level_height * 2.0;

  p[count] = {{-w, -h}, {w * 2.0f, h * 2.0f}};
  u[count] = {};
  c[count] = {0, 0, 0, 0.9};
  m[count] = {1, 1, 1, 1};

  return count + 1;
}

void open_menu() {
  using namespace casein;
  reset_g(GESTURE);
  reset_k(KEY_DOWN);

  handle(KEY_DOWN, K_ESCAPE, &setup_game);

  quack::donald::data(update_data);
}

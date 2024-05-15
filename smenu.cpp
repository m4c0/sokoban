module sokoban;

static unsigned update_data(quack::mapped_buffers all) {
  auto count = sr::update_data(all);

  auto [c, m, p, u] = all;

  float w = sl::level_width;
  float h = sl::level_height;

  p[count] = {{0, 0}, {w, h}};
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

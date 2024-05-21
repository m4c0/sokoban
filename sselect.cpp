module sokoban;

static unsigned update_data(quack::mapped_buffers all) {
  auto count = ui::menu_bg(all, 10, 10);
  return count;
}

void open_level_select() {
  using namespace casein;
  reset_g(GESTURE);
  reset_k(KEY_DOWN);

  quack::donald::data(update_data);
}

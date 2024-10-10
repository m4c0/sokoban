module sokoban;

void ui::bg(quack::instance *& all, float w, float h) {
  float x = (sl::level_width - w) * 0.5f;
  float y = (sl::level_height - h) * 0.5f;
  spr::blit::selection(all, x, y, w, h);
}

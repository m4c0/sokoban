module sokoban;

void ui::bg(quack::instance *& all, float w, float h, dotz::vec4 cl) {
  float x = sl::level_width - w;
  float y = sl::level_height - h;

  *all++ = quack::instance{
      .position{x * 0.5f, y * 0.5f},
      .size{w, h},
      .colour{cl},
      .multiplier{1},
  };
}

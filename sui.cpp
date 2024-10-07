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

void ui::menu_bg(quack::instance *& all, float w, float h, dotz::vec2 & p, dotz::vec2 & s) {
  sr::update_data(all, { w / 2, h / 2 });

  float x = sl::level_width - w;
  float y = sl::level_height - h;

  p = dotz::vec2 { x, y } * 0.5f;
  s = dotz::vec2 { w, h };
}

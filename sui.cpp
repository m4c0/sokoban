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
  static constexpr const dotz::vec4 diag_bg{0.1f, 0.2f, 0.3f, 0.7f};

  sr::update_data(all, { w / 2, h / 2 });

  bg(all, w, h, diag_bg);

  p = all[-1].position + 2.0f;
  s = all[-1].size - 2.0f;
}

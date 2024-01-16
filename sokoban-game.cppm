export module sokoban:game;
import :atlas;
import :audio;
import :grid;
import :levels;
import :vulkan;

class game_grid {
  grid m_grid{};
  unsigned m_p{};
  unsigned m_level{};

  static constexpr const auto width = level_width;

  void render() { renderer::instance().render(m_grid, m_p); }

  void move(unsigned p) {
    switch (auto mt = m_grid.move_type(m_p, p)) {
    case none:
      streamer::instance().play(150);
      return;
    case push:
    case push2tgt:
      m_p += p;
      m_grid.set_box(m_p + p);
      m_grid.clear_box(m_p);
      if (m_grid.is_done()) {
        set_level(m_level + 1);
      } else if (mt == push2tgt) {
        streamer::instance().play(100);
      } else {
        streamer::instance().play(200);
      }
      break;
    case walk:
      m_p += p;
      break;
    }
    render();
  }

public:
  void set_level(unsigned idx) {
    m_level = idx % max_levels;

    auto lvl = levels[idx];
    m_grid.load(lvl);

    m_p = 0;
    while (lvl[m_p] != 'P')
      m_p++;

    render();
    streamer::instance().play(50);
  }
  void reset_level() { set_level(m_level); }

  void down() { move(width); }
  void up() { move(-width); }
  void left() { move(-1); }
  void right() { move(1); }
};

module sokoban;

namespace sgg = sokoban::game_grid;

static grid &m_grid = grid::instance();
static unsigned &m_p = grid::instance().player_pos();
static unsigned m_level{};

static constexpr const auto width = level_width;

static void render() { renderer::instance().render(); }

static void move(unsigned p) {
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
      sgg::set_level(m_level + 1);
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

void sgg::set_level(unsigned idx) {
  m_level = idx % max_levels;

  auto lvl = levels[idx];
  m_grid.load(lvl);

  m_p = 0;
  while (lvl[m_p] != 'P')
    m_p++;

  render();
  streamer::instance().play(50);
}
void sgg::reset_level() { set_level(m_level); }

void sgg::down() { move(width); }
void sgg::up() { move(-width); }
void sgg::left() { move(-1); }
void sgg::right() { move(1); }

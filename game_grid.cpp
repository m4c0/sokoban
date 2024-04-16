module sokoban;
import :grid;

namespace sg = sokoban::game;
namespace sgg = sokoban::game_grid;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static unsigned m_level{};

static void move(unsigned p) {
  switch (auto mt = sg::grid.move_type(sg::player_pos, p)) {
  case none:
    streamer::instance().play(150);
    return;
  case push:
  case push2tgt:
    sg::player_pos += p;
    sg::grid.set_box(sg::player_pos + p);
    sg::grid.clear_box(sg::player_pos);
    if (sg::grid.is_done()) {
      sgg::set_level(m_level + 1);
    } else if (mt == push2tgt) {
      streamer::instance().play(100);
    } else {
      streamer::instance().play(200);
    }
    break;
  case walk:
    sg::player_pos += p;
    break;
  }
  sr::render();
}

void sgg::set_level(unsigned idx) {
  m_level = idx % sl::max_levels();

  auto lvl = sl::level(idx);
  sg::grid.load(lvl);

  sg::player_pos = 0;
  while (lvl[sg::player_pos] != 'P')
    sg::player_pos++;

  sr::render();
  streamer::instance().play(50);
}
void sgg::reset_level() { set_level(m_level); }

void sgg::down() { move(sl::level_width); }
void sgg::up() { move(-sl::level_width); }
void sgg::left() { move(-1); }
void sgg::right() { move(1); }

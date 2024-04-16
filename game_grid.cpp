module sokoban;
import :grid;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static unsigned m_level{};

void set_level(unsigned idx) {
  m_level = idx % sl::max_levels();

  auto lvl = sl::level(idx);
  sg::grid.load(lvl);

  sg::player_pos = 0;
  while (lvl[sg::player_pos] != 'P')
    sg::player_pos++;

  sr::render();
  streamer::instance().play(50);
}

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
      set_level(m_level + 1);
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

void reset_level() { set_level(m_level); }

void down() { move(sl::level_width); }
void up() { move(-sl::level_width); }
void left() { move(-1); }
void right() { move(1); }

// TODO: use casein's map<void>
static class : public casein::handler {
public:
  void create_window(const casein::events::create_window &e) override {
    set_level(0);
  }
  void gesture(const casein::events::gesture &e) override {
    switch (*e) {
    case casein::G_SWIPE_UP:
      up();
      break;
    case casein::G_SWIPE_DOWN:
      down();
      break;
    case casein::G_SWIPE_LEFT:
      left();
      break;
    case casein::G_SWIPE_RIGHT:
      right();
      break;
    case casein::G_SHAKE:
      reset_level();
      break;
    default:
      break;
    }
  }
  void key_down(const casein::events::key_down &e) override {
    switch (*e) {
    case casein::K_UP:
      up();
      break;
    case casein::K_DOWN:
      down();
      break;
    case casein::K_LEFT:
      left();
      break;
    case casein::K_RIGHT:
      right();
      break;
    case casein::K_SPACE:
      reset_level();
      break;
    default:
      break;
    }
  }
} evt;
void sokoban::game_grid::process_event(const casein::event &e) {
  evt.handle(e);
}

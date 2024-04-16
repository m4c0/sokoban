#pragma leco app
#pragma leco add_impl game_grid
#pragma leco add_impl levels
#pragma leco add_impl quack

export module sokoban;
import :audio;
import :grid;
import casein;
import quack;

namespace sokoban::game_grid {
void set_level(unsigned);
void reset_level();

void down();
void up();
void left();
void right();
} // namespace sokoban::game_grid

namespace sokoban::levels {
extern const unsigned level_width;
extern const unsigned level_height;
unsigned max_levels();
const char *level(unsigned);
} // namespace sokoban::levels

namespace sokoban::renderer {
void process_event(const casein::event &e);
void render();
} // namespace sokoban::renderer

namespace sokoban::game {
extern class grid grid;
extern unsigned player_pos;
} // namespace sokoban::game

static class : public casein::handler {
public:
  void create_window(const casein::events::create_window &e) override {
    sokoban::game_grid::set_level(0);
  }
  void gesture(const casein::events::gesture &e) override {
    switch (*e) {
    case casein::G_SWIPE_UP:
      sokoban::game_grid::up();
      break;
    case casein::G_SWIPE_DOWN:
      sokoban::game_grid::down();
      break;
    case casein::G_SWIPE_LEFT:
      sokoban::game_grid::left();
      break;
    case casein::G_SWIPE_RIGHT:
      sokoban::game_grid::right();
      break;
    case casein::G_SHAKE:
      sokoban::game_grid::reset_level();
      break;
    default:
      break;
    }
  }
  void key_down(const casein::events::key_down &e) override {
    switch (*e) {
    case casein::K_UP:
      sokoban::game_grid::up();
      break;
    case casein::K_DOWN:
      sokoban::game_grid::down();
      break;
    case casein::K_LEFT:
      sokoban::game_grid::left();
      break;
    case casein::K_RIGHT:
      sokoban::game_grid::right();
      break;
    case casein::K_SPACE:
      sokoban::game_grid::reset_level();
      break;
    default:
      break;
    }
  }
} evt;

extern "C" void casein_handle(const casein::event &e) {
  streamer::instance();
  quack::mouse_tracker::instance().handle(e);
  sokoban::renderer::process_event(e);
  evt.handle(e);
}

module :private;

namespace sokoban::game {
class grid grid {};
unsigned player_pos{};
} // namespace sokoban::game

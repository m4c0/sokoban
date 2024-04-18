#pragma leco app
#pragma leco add_impl audio
#pragma leco add_impl game_grid
#pragma leco add_impl levels
#pragma leco add_impl quack

export module game;
import :grid;
import casein;
import quack;

// TODO: move each of these into its own module

namespace sokoban::audio {
void play(unsigned);
}

namespace sokoban::game_grid {
void process_event(const casein::event &e);
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

extern "C" void casein_handle(const casein::event &e) {
  quack::mouse_tracker::instance().handle(e);
  sokoban::renderer::process_event(e);
  sokoban::game_grid::process_event(e);
}

module :private;

namespace sokoban::game {
class grid grid {};
unsigned player_pos{};
} // namespace sokoban::game

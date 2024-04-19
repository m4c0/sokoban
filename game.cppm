#pragma leco add_impl audio
#pragma leco add_impl game_grid
#pragma leco add_impl levels
#pragma leco add_impl quack

export module game;
import :grid;
import quack;

// TODO: move each of these into its own module

namespace sokoban::audio {
void play(unsigned);
}

export namespace sokoban::game_grid {
void init();
void set_level(unsigned);
} // namespace sokoban::game_grid

export namespace sokoban::levels {
extern const unsigned level_width;
extern const unsigned level_height;
unsigned max_levels();
const char *level(unsigned);
} // namespace sokoban::levels

export namespace sokoban::renderer {
void render();
} // namespace sokoban::renderer

export namespace sokoban::game {
extern class grid grid;
extern unsigned player_pos;
} // namespace sokoban::game

module :private;

namespace sokoban::game {
class grid grid {};
unsigned player_pos{};
} // namespace sokoban::game

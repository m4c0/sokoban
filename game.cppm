#pragma leco add_impl levels
#pragma leco add_impl renderer

export module game;
import jute;
import missingno;
import yoyo;
import quack;
import siaudio;
import voo;

export namespace sokoban::enums {
enum blocks : char {
  player = 'P',
  player_target = 'p',
  wall = 'X',
  empty = '.',
  outside = ' ',
  target = '*',
  box = 'O',
  target_box = '0',
};
}

export namespace sokoban::levels {
extern const unsigned level_width;
extern const unsigned level_height;
unsigned current_level();
unsigned max_levels();
jute::view level(unsigned);
void load_level(jute::view lvl, unsigned lvl_number);
void load_level(unsigned l);
mno::req<void> read_levels(yoyo::reader *r);

inline unsigned level_quad_count() { return level_width * level_height; }
} // namespace sokoban::levels

export namespace sokoban::renderer {
unsigned update_data(quack::mapped_buffers all);
} // namespace sokoban::renderer

export namespace sokoban::game {
constexpr const auto max_quads = 1024;
extern enums::blocks grid[max_quads];
extern unsigned player_pos;
} // namespace sokoban::game

module :private;

namespace sokoban::game {
enums::blocks grid[max_quads];
unsigned player_pos{};
} // namespace sokoban::game

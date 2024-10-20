#pragma leco add_impl levels
#pragma leco add_impl renderer

export module game;
import dotz;
import hai;
import jute;
import missingno;
import yoyo;
import quack;
import siaudio;
import sprites;
import voo;

export namespace sokoban::levels {
extern const unsigned level_width;
extern const unsigned level_height;
unsigned current_level();
unsigned max_levels();
jute::view level(unsigned);
void load_level(jute::view lvl, unsigned lvl_number);
void load_level(unsigned l);
void read_levels();

inline unsigned level_quad_count() { return level_width * level_height; }
} // namespace sokoban::levels

export namespace sokoban::renderer {
  extern voo::device_and_queue * g_dq;
void update_data(quack::instance *& i, dotz::vec2 menu_sz, dotz::vec4 sel, float back_btn, float menu_btn);
void set_updater(hai::fn<void, quack::instance *&>);
dotz::vec2 mouse_pos();
} // namespace sokoban::renderer

export namespace sokoban::game {
constexpr const auto max_quads = 1024;
extern spr::blocks grid[max_quads];
extern unsigned player_pos;
} // namespace sokoban::game

module :private;

namespace sokoban::game {
spr::blocks grid[max_quads];
unsigned player_pos{};
} // namespace sokoban::game

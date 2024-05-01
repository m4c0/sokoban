#pragma leco add_impl audio
#pragma leco add_impl levels
#pragma leco add_impl renderer

export module game;
import jute;
import quack;
import siaudio;
import voo;

// TODO: move each of these into its own module

export namespace sokoban::audio {
class streamer : siaudio::os_streamer {
  void fill_buffer(float *data, unsigned samples) override;

public:
  void play(unsigned);
};
} // namespace sokoban::audio

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
void load_level(jute::view lvl);

inline unsigned level_quad_count() { return level_width * level_height; }
} // namespace sokoban::levels

export namespace sokoban::renderer {
struct rnd : public quack::donald {
  unsigned max_quads() const noexcept override;
  unsigned quad_count() const noexcept override;
  quack::upc push_constants() const noexcept override;
  virtual void update_data(quack::mapped_buffers all) override;
  atlas create_atlas(voo::device_and_queue *dq) override;
};
} // namespace sokoban::renderer

export namespace sokoban::game {
extern enums::blocks grid[1024];
extern unsigned player_pos;
} // namespace sokoban::game

module :private;

namespace sokoban::game {
enums::blocks grid[1024];
unsigned player_pos{};
} // namespace sokoban::game

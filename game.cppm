#pragma leco add_impl audio
#pragma leco add_impl levels
#pragma leco add_impl renderer

export module game;
import :grid;
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
}

export namespace sokoban::levels {
extern const unsigned level_width;
extern const unsigned level_height;
unsigned current_level();
unsigned max_levels();
jute::view level(unsigned);
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
extern class grid grid;
extern unsigned player_pos;
void set_level(jute::view lvl);
} // namespace sokoban::game

module :private;

namespace sokoban::game {
class grid grid {};
unsigned player_pos{};
void set_level(jute::view lvl) {
  // TODO: assert lvl is smaller than our buffer
  auto ptr = grid.begin();
  for (auto c : lvl) {
    switch (c) {
    case player:
      *ptr++ = empty;
      break;
    case player_target:
      *ptr++ = target;
      break;
    default:
      *ptr++ = static_cast<blocks>(c);
      break;
    }
  }

  player_pos = 0;
  for (auto c : lvl) {
    if (c == player || c == player_target)
      break;
    player_pos++;
  }
}
} // namespace sokoban::game

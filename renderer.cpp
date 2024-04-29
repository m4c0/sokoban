#pragma leco add_resource "atlas.png"
module game;
import :atlas;
import :grid;
import casein;
import quack;
import vee;
import voo;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

// {{{ quad map utils
static constexpr auto uv(atlas_sprites s) {
  constexpr const auto h = 1.0f / static_cast<float>(sprite_count);
  const auto n = static_cast<unsigned>(s);
  return quack::uv{{0, n * h}, {1, (n + 1) * h}};
}
static constexpr auto uv(char b) {
  switch (b) {
  case box:
  case target_box:
    return uv(sprite_box);
  case target:
    return uv(sprite_target);
  case player:
  case player_target:
    return uv(sprite_player);
  default:
    return uv(sprite_empty);
  }
}
static constexpr auto colour(char b) {
  switch (b) {
  case target_box:
  case target:
  case player_target:
    return quack::colour{0, 0.7, 0, 1};
  case wall:
    return quack::colour{0, 0, 1, 1};
  case outside:
    return quack::colour{};
  case box:
  case player:
  case empty:
    return quack::colour{0, 0.3, 0, 1};
  }
  return quack::colour{1, 0, 1, 1};
}
// }}}

// TODO: make this a common const between quack, maped and grid
unsigned sr::rnd::max_quads() const noexcept { return 1024; }
unsigned sr::rnd::quad_count() const noexcept {
  return sl::level_width * sl::level_height;
}

quack::upc sr::rnd::push_constants() const noexcept {
  quack::upc rpc{};
  rpc.grid_size = {sl::level_width, sl::level_height};
  rpc.grid_pos = rpc.grid_size / 2.0;
  return rpc;
}

void sr::rnd::update_data(quack::mapped_buffers all) {
  auto [c, m, p, u] = all;
  auto i = 0U;
  for (char b : sg::grid) {
    if (sg::player_pos == i) {
      b = (b == target) ? player_target : player;
    }
    float x = i % sl::level_width;
    float y = i / sl::level_width;
    *p++ = {{x, y}, {1, 1}};
    *u++ = uv(b);
    *c++ = colour(b);
    *m++ = quack::colour{1, 1, 1, 1};
    i++;
  }
}

sr::rnd::atlas sr::rnd::create_atlas(voo::device_and_queue *dq) {
  return atlas{new voo::sires_image("atlas.png", dq)};
}

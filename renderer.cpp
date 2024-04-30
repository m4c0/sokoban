#pragma leco add_resource "atlas.png"
module game;
import casein;
import quack;
import vee;
import voo;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;
using namespace sokoban::enums;

enum atlas_sprites {
  sprite_empty,
  sprite_player,
  sprite_box,
  sprite_target,
  sprite_count
};

// {{{ quad map utils
static constexpr auto uv(atlas_sprites s) {
  constexpr const auto h = 1.0f / static_cast<float>(sprite_count);
  const auto n = static_cast<unsigned>(s);
  return quack::uv{{0, n * h}, {0.25, (n + 1) * h}};
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
  return sl::level_width * sl::level_height + 3;
}

quack::upc sr::rnd::push_constants() const noexcept {
  quack::upc rpc{};
  rpc.grid_size = {sl::level_width, sl::level_height};
  rpc.grid_pos = rpc.grid_size / 2.0;
  return rpc;
}

void sr::rnd::update_data(quack::mapped_buffers all) {
  auto [c, m, p, u] = all;

  for (auto i = 0U; i < sl::level_quad_count(); i++) {
    auto b = sg::grid[i];
    if (sg::player_pos == i) {
      b = (b == target) ? player_target : player;
    }
    float x = i % sl::level_width;
    float y = i / sl::level_width;
    *p++ = {{x, y}, {1, 1}};
    *u++ = uv(b);
    *c++ = colour(b);
    *m++ = quack::colour{1, 1, 1, 1};
  }

  // Level
  *p++ = {{0, 0}, {2, 1}};
  *u++ = {{0.25, 0.0}, {0.75, 0.25}};
  *c++ = {0, 0, 0, 0};
  *m++ = {1, 1, 1, 1};

  // Number
  auto n = sl::current_level();
  auto x = 2.75f;
  for (auto i = 0; i < 3; i++) {
    auto d = n % 10;
    auto uu = (d % 6) / 8.0f;
    auto uv = (d / 6) / 4.0f;

    *p++ = {{x, 0}, {0.5, 1}};
    *u++ = {{0.25f + uu, 0.25f + uv}, {0.375f + uu, 0.5f + uv}};
    *c++ = {0, 0, 0, 0};
    *m++ = {1, 1, 1, 1};

    x -= 0.5;
    n /= 10;
  }
}

sr::rnd::atlas sr::rnd::create_atlas(voo::device_and_queue *dq) {
  return atlas{new voo::sires_image("atlas.png", dq)};
}

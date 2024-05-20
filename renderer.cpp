#pragma leco add_resource "atlas.png"
module game;
import casein;
import quack;
import sprites;
import vee;
import voo;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;
using namespace sokoban::enums;

enum atlas_sprites { sprite_empty, sprite_player, sprite_box, sprite_target };

// {{{ quad map utils
static constexpr auto uv(atlas_sprites s) {
  const auto n = static_cast<unsigned>(s);
  return quack::uv{{0, n * spr::h}, {spr::w, (n + 1) * spr::h}};
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
namespace {
static struct init {
  init() {
    quack::upc rpc{};
    rpc.grid_size = {sl::level_width, sl::level_height};
    rpc.grid_pos = rpc.grid_size / 2.0;

    using namespace quack::donald;
    max_quads(sg::max_quads);
    push_constants(rpc);
    atlas([](auto dq) -> atlas_t * {
      return new voo::sires_image("atlas.png", dq);
    });
  }
} i;
} // namespace

unsigned sr::update_data(quack::mapped_buffers &all) {
  auto &[c, m, p, u] = all;

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
  unsigned count = sl::level_width * sl::level_height;

  count += spr::blit::level(all, 0, 0);
  count += spr::blit::number(all, sl::current_level() + 1, 3.5, 0);
  return count;
}

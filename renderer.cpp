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

namespace {
static struct init {
  init() {
    quack::upc rpc{};
    rpc.grid_size = {sl::level_width, sl::level_height};
    rpc.grid_pos = rpc.grid_size / 2.0;

    using namespace quack::donald;
    max_quads(sg::max_quads);
    push_constants(rpc);
    atlas("atlas.png");
  }
} i;
} // namespace

void sr::update_data(quack::instance *& all) {
  for (auto i = 0U; i < sl::level_quad_count(); i++) {
    auto b = sg::grid[i];
    if (sg::player_pos == i) {
      b = (b == spr::target) ? spr::player_target : spr::player;
    }
    float x = i % sl::level_width;
    float y = i / sl::level_width;
    spr::blit::block(all, x, y, b);
  }

  spr::blit::level(all, 0, 0);
  spr::blit::number(all, sl::current_level() + 1, 3.5, 0);
}

void sr::set_updater(hai::fn<void, quack::instance *&> u) {
  quack::donald::data(u);
}

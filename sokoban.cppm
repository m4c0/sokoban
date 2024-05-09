#pragma leco app
#pragma leco add_impl splay
#pragma leco add_resource "levels.dat"
export module sokoban;
import casein;
import game;
import quack;
import silog;
import sires;

namespace sa = sokoban::audio;
namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;
using namespace sokoban::enums;

static sa::streamer audio{};

void setup_game(int level);
void refresh_batch() {
  using namespace quack::donald;
  data(sr::update_data);
}

struct init {
  init() {
    sires::open("levels.dat")
        .fmap([](auto &&r) { return sl::read_levels(&*r); })
        .take([](auto msg) {
          silog::log(silog::error, "failed to load levels data: %s", msg);
        });

    setup_game(0);

    using namespace quack::donald;
    app_name("sokoban");
    refresh_batch();
  }
} i;

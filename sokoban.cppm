#pragma leco app
#pragma leco add_impl splay
#pragma leco add_resource "levels.dat"
export module sokoban;
import casein;
import game;
import silog;
import sires;

namespace sa = sokoban::audio;
namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;
using namespace sokoban::enums;

static struct : sr::rnd {
  const char *app_name() const noexcept override { return "sokoban"; }
} r;

static sa::streamer audio{};

void setup_game(int level);

struct init {
  init() {
    sires::open("levels.dat")
        .fmap([](auto &&r) { return sl::read_levels(&*r); })
        .take([](auto msg) {
          silog::log(silog::error, "failed to load levels data: %s", msg);
        });

    setup_game(0);
  }
} i;

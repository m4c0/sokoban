#pragma leco app
#pragma leco add_impl smenu
#pragma leco add_impl splay
#pragma leco add_resource "levels.dat"
export module sokoban;
import buoy;
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

void setup_game();
void open_menu();

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

    int level = buoy::open_for_reading("sokoban", "save.dat")
                    .fmap([](auto &&r) { return r->read_u32(); })
                    .unwrap(0);
    sl::load_level(level);
    setup_game();

    using namespace quack::donald;
    app_name("sokoban");
    refresh_batch();
  }
} i;

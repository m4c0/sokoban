#pragma leco app
#pragma leco add_impl audio
#pragma leco add_impl smenu
#pragma leco add_impl sselect
#pragma leco add_impl splay
#pragma leco add_resource "levels.dat"
export module sokoban;
import buoy;
import casein;
import game;
import quack;
import silog;
import sires;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;
using namespace sokoban::enums;

void setup_game();
void open_menu();
void open_level_select();

bool is_audio_enabled();
void enable_audio(bool);
void play_tone(unsigned);

struct init {
  init() {
    enable_audio(true);

    quack::donald::app_name("sokoban");

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
  }
} i;

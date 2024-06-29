#pragma leco app
#pragma leco add_impl audio
#pragma leco add_impl smenu
#pragma leco add_impl splay
#pragma leco add_impl sselect
#pragma leco add_impl sui
#pragma leco add_resource "levels.dat"
#pragma leco add_build_dep "maped.cppm"
export module sokoban;
import casein;
import game;
import quack;
import save;
import silog;
import sires;
import sprites;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;
using namespace spr;

namespace ui {
unsigned bg(quack::mapped_buffers &all, float w, float h, quack::colour cl);
unsigned menu_bg(quack::mapped_buffers &all, float w, float h, quack::rect &r);
} // namespace ui

void setup_game();
void open_menu();
void open_level_select();

bool is_audio_enabled();
void enable_audio(bool);
void play_tone(unsigned);

struct init {
  init() {
    casein::set_title("Warehouse Game");
    enable_audio(true);

    quack::donald::app_name("sokoban");

    sl::read_levels();

    int level = save::read().cur_level;
    sl::load_level(level);
    setup_game();
  }
} i;

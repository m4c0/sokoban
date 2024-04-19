#pragma leco app
export module maped;
import casein;
import game;

namespace sgg = sokoban::game_grid;
namespace sr = sokoban::renderer;

static class : public casein::handler {
  void create_window(const casein::events::create_window &e) override {
    sgg::set_level(2);
  }
} i;

extern "C" void casein_handle(const casein::event &e) {
  sr::process_event(e);
  i.handle(e);
}

#pragma leco app
export module maped;
import casein;
import game;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static class : public casein::handler {
  void create_window(const casein::events::create_window &e) override {
    auto lvl = sl::level(0);
    sg::grid.load(lvl);

    sg::player_pos = 0;
    while (lvl[sg::player_pos] != 'P')
      sg::player_pos++;

    sr::render();
  }
} i;

extern "C" void casein_handle(const casein::event &e) {
  sr::process_event(e);
  i.handle(e);
}

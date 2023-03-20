export module sokoban;
import casein;
import quack;

class game_grid : public quack::grid_renderer<10, 10, bool> {
public:
  void fill_colour() {
    grid_renderer::fill_colour([](bool b) {
      return b ? quack::colour{1, 1, 1, 1} : quack::colour{};
    });
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static game_grid r{};

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.setup(e.as<casein::events::create_window>().native_window_handle());
    r.at(1, 1) = true;
    r.at(2, 1) = true;
    r.fill_colour();
    break;
  case casein::REPAINT:
    // if (g.tick())
    r.repaint();
    break;
  case casein::KEY_DOWN:
    switch (e.as<casein::events::key_down>().key()) {
    case casein::K_UP:
      // g.up();
      break;
    case casein::K_DOWN:
      // g.down();
      break;
    case casein::K_LEFT:
      // g.left();
      break;
    case casein::K_RIGHT:
      // g.right();
      break;
    case casein::K_SPACE:
      // g = {};
      break;
    default:
      break;
    }
    r.repaint();
    break;
  case casein::QUIT:
    r.quit();
    break;
  default:
    break;
  }
}

export module sokoban;
import casein;
import quack;

class game_grid : public quack::grid_renderer<12, 10, bool> {
  unsigned m_px{};
  unsigned m_py{};

public:
  void render() {
    reset_grid();
    at(m_px, m_py) = true;
    fill_colour([](bool b) {
      return b ? quack::colour{1, 1, 1, 1} : quack::colour{};
    });
  }

  void down() { m_py++; }
  void up() { m_py--; }
  void left() { m_px--; }
  void right() { m_px++; }
};

extern "C" void casein_handle(const casein::event &e) {
  static game_grid r{};

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.setup(e.as<casein::events::create_window>().native_window_handle());
    r.render();
    break;
  case casein::REPAINT:
    // if (g.tick())
    r.repaint();
    break;
  case casein::KEY_DOWN:
    switch (e.as<casein::events::key_down>().key()) {
    case casein::K_UP:
      r.up();
      break;
    case casein::K_DOWN:
      r.down();
      break;
    case casein::K_LEFT:
      r.left();
      break;
    case casein::K_RIGHT:
      r.right();
      break;
    default:
      break;
    }
    r.render();
    break;
  case casein::QUIT:
    r.quit();
    break;
  default:
    break;
  }
}

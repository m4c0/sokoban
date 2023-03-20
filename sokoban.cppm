export module sokoban;
import casein;
import quack;

template <auto W, auto H, typename Tp> class grid_renderer {
  quack::renderer m_r;
  Tp m_data[W * H];

public:
  static constexpr const auto width = W;
  static constexpr const auto height = H;
  static constexpr const auto cells = width * height;

  grid_renderer()
      : m_r(quack::params{
            .grid_w = W,
            .grid_h = H,
            .max_quads = W * H,
        }) {}

  void setup(casein::native_handle_t nptr) {
    m_r.setup(nptr);
    m_r.fill_pos([](quack::pos *is) {
      unsigned i = 0;
      for (auto y = 0; y < width; y++) {
        for (auto x = 0; x < height; x++, i++) {
          is[i].x = x;
          is[i].y = y;
        }
      }
    });
  }
  void repaint(auto &&fn) {
    m_r.fill_colour([&](auto *c) {
      for (auto i = 0; i < cells; i++) {
        c[i] = fn(at(i));
      }
    });
    m_r.repaint(cells);
  }
  void quit() { m_r.quit(); }

  [[nodiscard]] constexpr auto &at(unsigned x, unsigned y) noexcept {
    return m_data[y * W + x];
  }
  [[nodiscard]] constexpr auto &at(unsigned idx) noexcept {
    return m_data[idx];
  }
};

class game_grid : public grid_renderer<10, 10, bool> {};

extern "C" void casein_handle(const casein::event &e) {
  static game_grid r{};

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.setup(e.as<casein::events::create_window>().native_window_handle());
    r.at(1, 1) = true;
    r.at(2, 1) = true;
    r.repaint([](bool b) {
      return b ? quack::colour{1, 1, 1, 1} : quack::colour{};
    });
    break;
  case casein::REPAINT:
    // if (g.tick())
    r.repaint([](bool b) {
      return b ? quack::colour{1, 1, 1, 1} : quack::colour{};
    });
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
    // r.fill_colour(boa::ecs::grid2colour{g.grid()});
    break;
  case casein::QUIT:
    r.quit();
    break;
  default:
    break;
  }
}

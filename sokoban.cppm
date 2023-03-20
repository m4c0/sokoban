export module sokoban;
import casein;
import quack;

template <auto W, auto H, typename Tp>
class grid : public quack::filler<quack::pos> {
  Tp m_data[W * H];

public:
  static constexpr const auto width = W;
  static constexpr const auto height = H;
  static constexpr const auto cells = width * height;

  void operator()(quack::pos *is) const noexcept override {
    unsigned i = 0;
    for (auto y = 0; y < width; y++) {
      for (auto x = 0; x < height; x++, i++) {
        is[i].x = x;
        is[i].y = y;
      }
    }
  }

  [[nodiscard]] constexpr auto &at(unsigned x, unsigned y) noexcept {
    return m_data[y * W + x];
  }
  [[nodiscard]] constexpr auto &at(unsigned idx) noexcept {
    return m_data[idx];
  }

  operator quack::params() const noexcept {
    return quack::params{
        .grid_w = grid::width,
        .grid_h = grid::height,
        .max_quads = grid::cells,
    };
  }
};
template <typename Fn> static constexpr const auto colour_filler(Fn &&fn) {
  class s : public quack::filler<quack::colour> {
    Fn m;

  public:
    constexpr s(Fn &&fn) : m{fn} {}
    void operator()(quack::colour *c) const noexcept { m(c); }
  };
  return s{static_cast<Fn &&>(fn)};
}

extern "C" void casein_handle(const casein::event &e) {
  static grid<10, 10, bool> grd{};
  static quack::renderer r{grd};
  static constexpr const auto painter = [&](auto *c) {
    for (auto i = 0; i < decltype(grd)::cells; i++) {
      c[i] = grd.at(i) ? quack::colour{1, 1, 1, 1} : quack::colour{};
    }
  };

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.setup(e.as<casein::events::create_window>().native_window_handle());
    r.fill_pos(grd);
    grd.at(1, 1) = true;
    grd.at(2, 1) = true;

    r.fill_colour(colour_filler(painter));
    break;
  case casein::REPAINT:
    // if (g.tick())
    r.fill_colour(colour_filler(painter));
    r.repaint(decltype(grd)::cells);
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

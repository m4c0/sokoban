export module sokoban;
import casein;
import quack;

template <auto W, auto H> class grid : public quack::filler<quack::pos> {
public:
  static constexpr const auto width = W;
  static constexpr const auto height = H;
  static constexpr const auto cells = width * height;

  void operator()(quack::pos *p) const noexcept override {}
  operator quack::params() const noexcept {
    return quack::params{
        .grid_w = grid::width,
        .grid_h = grid::height,
        .max_quads = grid::cells,
    };
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static grid<10, 10> grd{};
  static quack::renderer r{grd};

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.setup(e.as<casein::events::create_window>().native_window_handle());
    r.fill_pos(grd);
    // r.fill_colour(boa::ecs::grid2colour{g.grid()});
    break;
  case casein::REPAINT:
    // if (g.tick()) r.fill_colour(boa::ecs::grid2colour{g.grid()});
    // r.repaint(boa::ecs::grid_cells);
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

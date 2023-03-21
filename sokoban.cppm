export module sokoban;
import casein;
import quack;

static constexpr const auto level_1 = "    XXXXX   "
                                      "  XXX   X   "
                                      "  X.PO  X   "
                                      "  XXX O.X   "
                                      "  X.XXO X   "
                                      "  X X . XX  "
                                      "  XO 0OO.X  "
                                      "  X   .  X  "
                                      "  XXXXXXXX  "
                                      "            ";

class grid {
  char m_buf[1024]{};
  char *m_end{};

public:
  constexpr void load(const char *l) {
    m_end = m_buf;
    while (*l) {
      *m_end++ = (*l == 'P') ? ' ' : *l;
      l++;
    }
  }

  [[nodiscard]] constexpr auto *begin() const noexcept { return m_buf; }
  [[nodiscard]] constexpr auto *end() const noexcept { return m_end; }
};

class game_grid : public quack::grid_renderer<12, 10, char> {
  grid m_grid{};
  unsigned m_px{};
  unsigned m_py{};

  void render() {
    unsigned i = 0;
    for (auto c : m_grid)
      at(i++) = c;
    at(m_px, m_py) = 'P';

    fill_colour([](char b) {
      switch (b) {
      case 'O':
        return quack::colour{1, 1, 0, 1};
      case '0':
        return quack::colour{0, 1, 0, 1};
      case '.':
        return quack::colour{0, 0.5, 0, 1};
      case 'P':
        return quack::colour{1, 0, 0, 1};
      case 'X':
        return quack::colour{0, 0, 1, 1};
      default:
        return quack::colour{};
      }
    });
  }

  void move(unsigned x, unsigned y) {
    switch (at(x, y)) {
    case 'X':
      return;
    }
    m_px = x;
    m_py = y;
    render();
  }

public:
  void set_level(const char *lvl) {
    m_grid.load(lvl);

    for (auto y = 0; y < height; y++)
      for (auto x = 0; x < width; x++, lvl++)
        if (*lvl == 'P') {
          m_px = x;
          m_py = y;
        }

    render();
  }

  void down() { move(m_px, m_py + 1); }
  void up() { move(m_px, m_py - 1); }
  void left() { move(m_px - 1, m_py); }
  void right() { move(m_px + 1, m_py); }
};

extern "C" void casein_handle(const casein::event &e) {
  static game_grid r{};

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.setup(e.as<casein::events::create_window>().native_window_handle());
    r.set_level(level_1);
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
    break;
  case casein::QUIT:
    r.quit();
    break;
  default:
    break;
  }
}

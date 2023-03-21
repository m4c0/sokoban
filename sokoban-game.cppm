export module sokoban:game;
import quack;

enum blocks : char {
  player = 'P',
  wall = 'X',
  empty = ' ',
  target = '.',
  box = 'O',
  target_box = '0',
};
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
  blocks m_buf[1024]{};
  blocks *m_end{};

public:
  constexpr void load(const char *l) {
    m_end = m_buf;
    while (*l) {
      *m_end++ = static_cast<blocks>((*l == 'P') ? ' ' : *l);
      l++;
    }
  }

  [[nodiscard]] constexpr auto *begin() const noexcept { return m_buf; }
  [[nodiscard]] constexpr auto *end() const noexcept { return m_end; }

  [[nodiscard]] constexpr auto operator[](unsigned p) const noexcept {
    return m_buf[p];
  }
};

class game_grid : public quack::grid_renderer<12, 10, blocks> {
  grid m_grid{};
  unsigned m_p{};

  void render() {
    unsigned i = 0;
    for (auto c : m_grid)
      at(i++) = c;

    at(m_p) = player;

    fill_colour([](char b) {
      switch (b) {
      case box:
        return quack::colour{1, 1, 0, 1};
      case target_box:
        return quack::colour{0, 1, 0, 1};
      case target:
        return quack::colour{0, 0.5, 0, 1};
      case player:
        return quack::colour{1, 0, 0, 1};
      case wall:
        return quack::colour{0, 0, 1, 1};
      case empty:
        return quack::colour{};
      }
      return quack::colour{1, 0, 1, 1};
    });
  }

  void move(unsigned p) {
    auto np = m_p + p;

    switch (m_grid[np]) {
    case box:
    case target_box:
      switch (m_grid[np + p]) {
      case empty:
      case target:
      case player:
        break;
      default:
        return;
      }
      break;
    case wall:
      return;
    case empty:
    case target:
    case player:
      break;
    }
    m_p = np;
    render();
  }

public:
  void set_level(const char *lvl) {
    m_grid.load(lvl);

    m_p = 0;
    while (lvl[m_p] != 'P')
      m_p++;

    render();
  }

  void down() { move(width); }
  void up() { move(-width); }
  void left() { move(-1); }
  void right() { move(1); }
};

export module sokoban:game;
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

  [[nodiscard]] constexpr auto operator[](unsigned p) const noexcept {
    return m_buf[p];
  }
};

class game_grid : public quack::grid_renderer<12, 10, char> {
  grid m_grid{};
  unsigned m_p{};

  void render() {
    unsigned i = 0;
    for (auto c : m_grid)
      at(i++) = c;

    at(m_p) = 'P';

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

  void move(unsigned p) {
    switch (m_grid[p]) {
    case '0':
    case 'O':
      break;
    case 'X':
      return;
    }
    m_p = p;
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

  void down() { move(m_p + width); }
  void up() { move(m_p - width); }
  void left() { move(m_p - 1); }
  void right() { move(m_p + 1); }
};

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

enum move_type { push, walk, none };

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

  [[nodiscard]] constexpr auto move_type(unsigned cur,
                                         unsigned delta) const noexcept {
    auto next = cur + delta;

    switch (m_buf[next]) {
    case box:
    case target_box:
      switch (m_buf[next + delta]) {
      case empty:
      case target:
      case player:
        return push;
      default:
        return none;
      }
    case wall:
      return none;
    case empty:
    case target:
    case player:
      return walk;
    }
  }

  constexpr void set_box(unsigned p) {
    m_buf[p] = (m_buf[p] == target) ? target_box : box;
  }
  constexpr void clear_box(unsigned p) {
    m_buf[p] = (m_buf[p] == target_box) ? target : empty;
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
    switch (m_grid.move_type(m_p, p)) {
    case none:
      return;
    case push:
      m_grid.set_box(m_p + p + p);
      m_grid.clear_box(m_p + p);
      break;
    case walk:
      break;
    }
    m_p += p;
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

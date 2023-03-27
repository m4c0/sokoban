export module sokoban:game;
import :atlas;
import :levels;
import quack;

enum blocks : char {
  player = 'P',
  wall = 'X',
  empty = '.',
  outside = ' ',
  target = '*',
  box = 'O',
  target_box = '0',
};

enum move_type { push, walk, none };

class grid {
  blocks m_buf[1024]{};
  blocks *m_end{};

public:
  constexpr void load(const char *l) {
    m_end = m_buf;
    while (*l) {
      *m_end++ = (*l == 'P') ? empty : static_cast<blocks>(*l);
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
    case outside:
    case wall:
      return none;
    case empty:
    case target:
    case player:
      return walk;
    }
  }

  [[nodiscard]] constexpr bool is_done() const noexcept {
    for (auto c : m_buf)
      if (c == target)
        return false;

    return true;
  }

  constexpr void set_box(unsigned p) {
    m_buf[p] = (m_buf[p] == target) ? target_box : box;
  }
  constexpr void clear_box(unsigned p) {
    m_buf[p] = (m_buf[p] == target_box) ? target : empty;
  }
};

class game_grid : public quack::grid_renderer<24, 12, blocks> {
  grid m_grid{};
  unsigned m_p{};
  unsigned m_level{};

  static constexpr auto uv(unsigned n) {
    constexpr const auto h = 1.0f / static_cast<float>(atlas_sprite_count);
    return quack::uv{{0, n * h}, {1, (n + 1) * h}};
  }

  void render() {
    unsigned i = 0;
    for (auto c : m_grid)
      at(i++) = c;

    at(m_p) = player;

    load_atlas(atlas_col_count, atlas_row_count, atlas{});
    fill_uv([](char b) {
      switch (b) {
      case player:
        return uv(1);
      default:
        return uv(0);
      }
    });
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
      case outside:
        return quack::colour{};
      case empty:
        return quack::colour{0, 0.3, 0, 1};
      }
      return quack::colour{1, 0, 1, 1};
    });
  }

  void move(unsigned p) {
    switch (m_grid.move_type(m_p, p)) {
    case none:
      return;
    case push:
      m_p += p;
      m_grid.set_box(m_p + p);
      m_grid.clear_box(m_p);
      if (m_grid.is_done()) {
        set_level(m_level + 1);
      }
      break;
    case walk:
      m_p += p;
      break;
    }
    render();
  }

public:
  void set_level(unsigned idx) {
    m_level = idx % max_levels;

    auto lvl = levels[idx];
    m_grid.load(lvl);

    m_p = 0;
    while (lvl[m_p] != 'P')
      m_p++;

    render();
  }
  void reset_level() { set_level(m_level); }

  void down() { move(width); }
  void up() { move(-width); }
  void left() { move(-1); }
  void right() { move(1); }
};

export module sokoban:grid;

enum blocks : char {
  player = 'P',
  player_target = 'p',
  wall = 'X',
  empty = '.',
  outside = ' ',
  target = '*',
  box = 'O',
  target_box = '0',
};

enum move_type { push, walk, none, push2tgt };

class grid {
  blocks m_buf[1024]{};
  blocks *m_end{};
  unsigned m_p;

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
        return push;
      case target:
        return push2tgt;
      default:
        return none;
      }
    case outside:
    case wall:
      return none;
    case empty:
    case target:
    case player:
    case player_target:
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

  [[nodiscard]] constexpr auto &player_pos() noexcept { return m_p; }

  static auto &instance() {
    static grid i{};
    return i;
  }
};

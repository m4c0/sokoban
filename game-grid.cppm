export module game:grid;
import jute;

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

public:
  constexpr void load(jute::view lvl) {
    // TODO: assert lvl is smaller than our buffer
    m_end = m_buf;
    for (auto c : lvl) {
      switch (c) {
      case player:
        *m_end++ = empty;
        break;
      case player_target:
        *m_end++ = target;
        break;
      default:
        *m_end++ = static_cast<blocks>(c);
        break;
      }
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
};

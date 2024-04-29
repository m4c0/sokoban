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

// TODO: move these to a place common between here and level
const unsigned level_width = 24;
const unsigned level_height = 14;

class grid {
  blocks m_buf[1024]{};

public:
  constexpr void load(jute::view lvl) {
    // TODO: assert lvl is smaller than our buffer
    auto ptr = m_buf;
    for (auto c : lvl) {
      switch (c) {
      case player:
        *ptr++ = empty;
        break;
      case player_target:
        *ptr++ = target;
        break;
      default:
        *ptr++ = static_cast<blocks>(c);
        break;
      }
    }
  }

  [[nodiscard]] constexpr auto *begin() const noexcept { return m_buf; }
  [[nodiscard]] constexpr auto *end() const noexcept {
    return m_buf + level_width * level_height;
  }

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

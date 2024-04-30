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

  [[nodiscard]] constexpr auto &operator[](unsigned idx) noexcept {
    return m_buf[idx];
  }
};

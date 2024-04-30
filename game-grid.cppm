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
  [[nodiscard]] constexpr auto &operator[](unsigned idx) noexcept {
    return m_buf[idx];
  }
};

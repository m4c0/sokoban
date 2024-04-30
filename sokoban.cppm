#pragma leco app
export module sokoban;
import casein;
import game;

namespace sa = sokoban::audio;
namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static unsigned m_level{};

static struct : sr::rnd {
  const char *app_name() const noexcept override { return "sokoban"; }
} r;

static sa::streamer audio{};

static void set_level(unsigned idx) {
  m_level = idx % sl::max_levels();
  sg::set_level(sl::level(idx));
  r.refresh_batch();
}

static void move(unsigned p) {
  switch (auto mt = sg::grid.move_type(sg::player_pos, p)) {
  case none:
    audio.play(150);
    return;
  case push:
  case push2tgt:
    sg::player_pos += p;
    sg::grid.set_box(sg::player_pos + p);
    sg::grid.clear_box(sg::player_pos);
    if (sg::grid.is_done()) {
      set_level(m_level + 1);
      audio.play(50);
    } else if (mt == push2tgt) {
      audio.play(100);
    } else {
      audio.play(200);
    }
    break;
  case walk:
    sg::player_pos += p;
    break;
  }
  r.refresh_batch();
}

void reset_level() { set_level(m_level); }

void down() { move(sl::level_width); }
void up() { move(-sl::level_width); }
void left() { move(-1); }
void right() { move(1); }

struct init {
  init() {
    sg::set_level(sl::level(m_level));

    using namespace casein;
    handle(GESTURE, G_SWIPE_UP, &up);
    handle(GESTURE, G_SWIPE_DOWN, &down);
    handle(GESTURE, G_SWIPE_LEFT, &left);
    handle(GESTURE, G_SWIPE_RIGHT, &right);
    handle(GESTURE, G_SHAKE, &reset_level);
    handle(KEY_DOWN, K_UP, &up);
    handle(KEY_DOWN, K_DOWN, &down);
    handle(KEY_DOWN, K_LEFT, &left);
    handle(KEY_DOWN, K_RIGHT, &right);
    handle(KEY_DOWN, K_SPACE, &reset_level);
  }
} i;

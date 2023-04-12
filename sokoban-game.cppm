export module sokoban:game;
import :atlas;
import :levels;
import quack;
import siaudio;

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

void play_sound(unsigned div) {
  static volatile unsigned sp = 0;
  static volatile unsigned d = 0;
  static auto s = siaudio::streamer{[&](float *data, unsigned samples) {
    auto ssp = sp;
    float mult;
    if (ssp < 1000) {
      mult = ssp / 1000.0f;
    } else if (ssp < 4000) {
      mult = 1.0;
    } else if (ssp < 5000) {
      mult = (5000 - ssp) / 1000.0f;
    } else {
      mult = 0;
    }
    for (unsigned i = 0; i < samples; ++i) {
      data[i] = 0.25f * mult * ((ssp / d) % 2) - 0.5f;
      ssp++;
    }
    sp = ssp;
  }};
  d = div;
  sp = 0;
}

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

class game_grid : public quack::grid_renderer<24, 12, blocks> {
  grid m_grid{};
  unsigned m_p{};
  unsigned m_level{};

  static constexpr auto uv(unsigned n) {
    constexpr const auto h = 1.0f / static_cast<float>(sprite_count);
    return quack::uv{{0, n * h}, {1, (n + 1) * h}};
  }

  void render() {
    unsigned i = 0;
    for (auto c : m_grid)
      at(i++) = c;

    at(m_p) = at(m_p) == target ? player_target : player;

    load_atlas(atlas_col_count, atlas_row_count, atlas{});
    fill_uv([](char b) {
      switch (b) {
      case box:
      case target_box:
        return uv(sprite_box);
      case target:
        return uv(sprite_target);
      case player:
      case player_target:
        return uv(sprite_player);
      default:
        return uv(sprite_empty);
      }
    });
    fill_colour([](char b) {
      switch (b) {
      case target_box:
      case target:
      case player_target:
        return quack::colour{0, 0.7, 0, 1};
      case wall:
        return quack::colour{0, 0, 1, 1};
      case outside:
        return quack::colour{};
      case box:
      case player:
      case empty:
        return quack::colour{0, 0.3, 0, 1};
      }
      return quack::colour{1, 0, 1, 1};
    });
  }

  void move(unsigned p) {
    switch (auto mt = m_grid.move_type(m_p, p)) {
    case none:
      play_sound(150);
      return;
    case push:
    case push2tgt:
      m_p += p;
      m_grid.set_box(m_p + p);
      m_grid.clear_box(m_p);
      if (m_grid.is_done()) {
        set_level(m_level + 1);
      } else if (mt == push2tgt) {
        play_sound(100);
      } else {
        play_sound(200);
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
    play_sound(50);
  }
  void reset_level() { set_level(m_level); }

  void down() { move(width); }
  void up() { move(-width); }
  void left() { move(-1); }
  void right() { move(1); }
};

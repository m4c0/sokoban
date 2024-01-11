export module sokoban:game;
import :audio;
import :atlas;
import :grid;
import :levels;
import quack;

class game_grid {
  quack::renderer m_r{1};
  quack::grid_ilayout<24, 12, blocks> m_g{&m_r};
  grid m_grid{};
  unsigned m_p{};
  unsigned m_level{};

  static constexpr const auto width = decltype(m_g)::width;

  static constexpr auto uv(unsigned n) {
    constexpr const auto h = 1.0f / static_cast<float>(sprite_count);
    return quack::uv{{0, n * h}, {1, (n + 1) * h}};
  }

  void render() {
    unsigned i = 0;
    for (auto c : m_grid)
      m_g.at(i++) = c;

    m_g.at(m_p) = m_g.at(m_p) == target ? player_target : player;

    m_g.batch()->load_atlas(atlas_col_count, atlas_row_count, atlas{});
    m_g.fill_uv([](char b) {
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
    m_g.fill_colour([](char b) {
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
    m_g.fill_mult([](char b) { return quack::colour{1, 1, 1, 1}; });
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

  void process_event(const auto &e) {
    m_r.process_event(e);
    m_g.process_event(e);
  }
};

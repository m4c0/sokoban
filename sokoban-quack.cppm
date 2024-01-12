export module sokoban:quack;
import :atlas;
import :grid;
import :levels;
import casein;
import quack;

class renderer {
  quack::renderer m_r{1};
  quack::ilayout m_il{&m_r, level_width *level_height};
  // quack::grid_ilayout<level_width, level_height, blocks> m_g{&m_r};

  static constexpr auto uv(atlas_sprites s) {
    constexpr const auto h = 1.0f / static_cast<float>(sprite_count);
    const auto n = static_cast<unsigned>(s);
    return quack::uv{{0, n * h}, {1, (n + 1) * h}};
  }
  static constexpr auto uv(char b) {
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
  }
  static constexpr auto colour(char b) {
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
  }

  renderer() = default;

  void setup() {
    m_il->set_grid(level_width, level_height);
    m_il->center_at(level_width / 2, level_height / 2);
    m_il->set_count(level_width * level_height);
    m_il->map_positions([](auto *is) {
      unsigned i = 0;
      for (float y = 0; y < level_height; y++) {
        for (float x = 0; x < level_width; x++, i++) {
          is[i] = {{x, y}, {1, 1}};
        }
      }
    });
  };

public:
  void render(const grid &g, unsigned p) {
    m_il->load_atlas(atlas_col_count, atlas_row_count, atlas{});
    m_il->map_all([&](auto all) {
      auto [c, m, _, u] = all;
      auto i = 0U;
      for (char b : g) {
        if (p == i) {
          b = (b == target) ? player_target : player;
        }
        *u++ = uv(b);
        *c++ = colour(b);
        *m++ = quack::colour{1, 1, 1, 1};
        i++;
      }
    });
  }

  void process_event(const auto &e) {
    m_r.process_event(e);
    m_il.process_event(e);

    if (e.type() == casein::CREATE_WINDOW) {
      setup();
    }
  }

  static auto &instance() noexcept {
    static renderer r{};
    return r;
  }
};

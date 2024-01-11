export module sokoban:quack;
import :atlas;
import :grid;
import :levels;
import quack;

class renderer {
  quack::renderer m_r{1};
  quack::grid_ilayout<level_width, level_height, blocks> m_g{&m_r};

  static constexpr auto uv(unsigned n) {
    constexpr const auto h = 1.0f / static_cast<float>(sprite_count);
    return quack::uv{{0, n * h}, {1, (n + 1) * h}};
  }

  renderer() = default;

public:
  void render(const grid &g, unsigned p) {
    unsigned i = 0;
    for (auto c : g)
      m_g.at(i++) = c;

    m_g.at(p) = m_g.at(p) == target ? player_target : player;

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

  void process_event(const auto &e) {
    m_r.process_event(e);
    m_g.process_event(e);
  }

  static auto &instance() noexcept {
    static renderer r{};
    return r;
  }
};

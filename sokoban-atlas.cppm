export module sokoban:atlas;
import quack;

constexpr const auto atlas_col_count = 8;
constexpr const auto atlas_row_count = 8;

using pal_line = unsigned[atlas_col_count];
constexpr const pal_line paletted_atlas[atlas_row_count]{
    {0, 1, 2, 3, 4, 5, 6, 7},
    {0, 1, 2, 3, 4, 5, 6, 7},
    {0, 1, 2, 3, 4, 5, 6, 7},
    {0, 1, 2, 3, 4, 5, 6, 7},
};
constexpr const quack::u8_rgba palette[]{
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
};

struct atlas_t {
  quack::u8_rgba data[atlas_row_count * atlas_col_count];
};
constexpr const atlas_t atlas = [] {
  atlas_t res{};
  for (auto i = 0; i < atlas_row_count; i++) {
    const auto &row = paletted_atlas[i];
    for (auto j = 0; j < atlas_col_count; j++) {
      const auto rgba = palette[row[j]];
      res.data[i * atlas_col_count + j] = rgba;
    }
  }
  return res;
}();

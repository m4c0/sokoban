export module sokoban:atlas;

enum atlas_sprites {
  sprite_empty,
  sprite_player,
  sprite_box,
  sprite_target,
  sprite_count
};
constexpr const auto atlas_col_count = 8;
constexpr const auto atlas_row_count = 8 * sprite_count;
constexpr const auto atlas_pixel_count = atlas_col_count * atlas_row_count;

using pal_line = char[atlas_col_count + 1];
constexpr const pal_line paletted_atlas[atlas_row_count]{
    "00000000", // flat colour
    "00000000", //
    "00000000", //
    "00000000", //
    "00000000", //
    "00000000", //
    "00000000", //
    "00000000", //
    "00011000", // player
    "00011000", //
    "01111110", //
    "01011010", //
    "01011010", //
    "00111100", //
    "00100100", //
    "00100100", //
    "11111111", // box
    "11000011", //
    "10100101", //
    "10011001", //
    "10011001", //
    "10100101", //
    "11000011", //
    "11111111", //
    "00000000", // target
    "00000000", //
    "00000000", //
    "00011000", //
    "00011000", //
    "00000000", //
    "00000000", //
    "00000000", //
};

struct atlas_r8 {
  unsigned char data[atlas_pixel_count];
};
constexpr auto atlas() {
  atlas_r8 res{};
  for (auto i = 0; i < atlas_row_count; i++) {
    const auto &row = paletted_atlas[i];
    for (auto j = 0; j < atlas_col_count; j++) {
      const auto rgba = row[j] == '0' ? 0 : 255;
      res.data[i * atlas_col_count + j] = rgba;
    }
  }
  return res;
}

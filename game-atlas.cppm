export module game:atlas;
import vee;
import voo;

enum atlas_sprites {
  sprite_empty,
  sprite_player,
  sprite_box,
  sprite_target,
  sprite_count
};
constexpr const auto atlas_col_count = 8U;
constexpr const auto atlas_row_count = 8U * sprite_count;
constexpr const auto atlas_pixel_count = atlas_col_count * atlas_row_count;

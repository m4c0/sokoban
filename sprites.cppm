export module sprites;
import dotz;
import quack;

export namespace spr {
// Shifts are a reminder for using POT textures to avoid FP imprecisions in GPU
constexpr const unsigned cols = 1 << 4;
constexpr const unsigned rows = 1 << 2;

constexpr const auto w = 1.0f / static_cast<float>(cols);
constexpr const auto h = 1.0f / static_cast<float>(rows);

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
} // namespace spr

namespace spr {
enum atlas_sprites { sprite_empty, sprite_player, sprite_box, sprite_target };
static constexpr auto uv(atlas_sprites s) {
  const auto n = static_cast<unsigned>(s);
  return dotz::vec4{0.0f, n * spr::h, spr::w, (n + 1) * spr::h};
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
    return dotz::vec4{0.f, 0.7f, 0.f, 1.f};
  case wall:
  case outside:
  case box:
  case player:
  case empty:
    return dotz::vec4{};
  }
  return dotz::vec4{1, 0, 1, 1};
}
} // namespace spr

export namespace spr::blit {
constexpr float operator""_w(long double n) { return n * w; }
constexpr float operator""_h(long double n) { return n * h; }
constexpr float operator""_w(unsigned long long n) { return n * w; }
constexpr float operator""_h(unsigned long long n) { return n * h; }

void digit(quack::instance *& i, unsigned d, float x, float y) {
  auto uu = (d % 6) * 0.5_w; // chars are half-w
  auto uv = (d / 6) * 1.0_h;

  *i++ = quack::instance{
      .position{x, y},
      .size{0.5f, 1.f},
      .uv0{1_w + uu, 1_h + uv},
      .uv1{1.5_w + uu, 2_h + uv},
      .multiplier{1, 1, 1, 1},
  };
}
void number(quack::instance *& all, unsigned n, float x, float y) {
  for (auto i = 0; i < 2; i++) {
    digit(all, n % 10, x, y);

    x -= 0.5;
    n /= 10;
  }
}

void boolean(quack::instance *& i, bool val, float x, float y) {
  float v = val ? 2_h : 3_h;

  *i++ = quack::instance{
      .position{x, y},
      .size{2, 1},
      .uv0{4_w, v},
      .uv1{6_w, v + 1_h},
      .multiplier{1, 1, 1, 1},
  };
}

void level(quack::instance *& i, float x, float y) {
  *i++ = quack::instance{
      .position{x, y},
      .size{3, 1},
      .uv0{1_w, 0_h},
      .uv1{4_w, 1_h},
      .multiplier{1, 1, 1, 1},
  };
}
void sound(quack::instance *& i, float x, float y) {
  *i++ = quack::instance{
      .position{x, y},
      .size{3, 1},
      .uv0{4_w, 0_h},
      .uv1{7_w, 1_h},
      .multiplier{1, 1, 1, 1},
  };
}
void fullscreen(quack::instance *& i, float x, float y) {
  *i++ = quack::instance{
      .position{x, y},
      .size{5, 1},
      .uv0{4_w, 1_h},
      .uv1{9_w, 2_h},
      .multiplier{1, 1, 1, 1},
  };
}

void cursor(quack::instance *& i, float x, float y) {
  *i++ = {
      .position {x, y},
      .size {1, 1},
      .colour {0, 0, 0, 1},
  };
}

void block(quack::instance *& i, float x, float y, blocks b) {
  auto uv = spr::uv(b);
  *i++ = {
      .position{x, y},
      .size{1, 1},
      .uv0{uv.x, uv.y},
      .uv1{uv.z, uv.w},
      .colour{colour(b)},
      .multiplier{1, 1, 1, 1},
  };
}
} // namespace spr::blit

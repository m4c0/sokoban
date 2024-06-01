export module sprites;
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
  return quack::uv{{0.0f, n * spr::h}, {spr::w, (n + 1) * spr::h}};
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
} // namespace spr

export namespace spr::blit {
constexpr float operator""_w(long double n) { return n * w; }
constexpr float operator""_h(long double n) { return n * h; }
constexpr float operator""_w(unsigned long long n) { return n * w; }
constexpr float operator""_h(unsigned long long n) { return n * h; }

unsigned digit(quack::mapped_buffers &all, unsigned d, float x, float y) {
  auto &[c, m, p, u] = all;
  auto uu = (d % 6) * 0.5_w; // chars are half-w
  auto uv = (d / 6) * 1.0_h;

  *p++ = {{x, y}, {0.5, 1}};
  *u++ = {{1_w + uu, 1_h + uv}, {1.5_w + uu, 2_h + uv}};
  *c++ = {0, 0, 0, 0};
  *m++ = {1, 1, 1, 1};
  return 1;
}
unsigned number(quack::mapped_buffers &all, unsigned n, float x, float y) {
  unsigned count{};
  for (auto i = 0; i < 2; i++) {
    count += digit(all, n % 10, x, y);

    x -= 0.5;
    n /= 10;
  }
  return count;
}

unsigned boolean(quack::mapped_buffers &all, bool val, float x, float y) {
  float v = val ? 2_h : 3_h;

  auto &[c, m, p, u] = all;
  *p++ = {{x, y}, {2, 1}};
  *u++ = {{4_w, v}, {6_w, v + 1_h}};
  *c++ = {0, 0, 0, 0};
  *m++ = {1, 1, 1, 1};
  return 1;
}

unsigned level(quack::mapped_buffers &all, float x, float y) {
  auto &[c, m, p, u] = all;
  *p++ = {{x, y}, {3, 1}};
  *u++ = {{1_w, 0_h}, {4_w, 1_h}};
  *c++ = {0, 0, 0, 0};
  *m++ = {1, 1, 1, 1};
  return 1;
}
unsigned sound(quack::mapped_buffers &all, float x, float y) {
  auto &[c, m, p, u] = all;
  *p++ = {{x, y}, {3, 1}};
  *u++ = {{4_w, 0_h}, {7_w, 1_h}};
  *c++ = {0, 0, 0, 0};
  *m++ = {1, 1, 1, 1};
  return 1;
}
unsigned fullscreen(quack::mapped_buffers &all, float x, float y) {
  auto &[c, m, p, u] = all;
  *p++ = {{x, y}, {5, 1}};
  *u++ = {{4_w, 1_h}, {9_w, 2_h}};
  *c++ = {0, 0, 0, 0};
  *m++ = {1, 1, 1, 1};
  return 1;
}

unsigned block(quack::mapped_buffers &all, float x, float y, blocks b) {
  auto &[c, m, p, u] = all;
  *p++ = {{x, y}, {1, 1}};
  *u++ = uv(b);
  *c++ = colour(b);
  *m++ = quack::colour{1, 1, 1, 1};
  return 1;
}
} // namespace spr::blit

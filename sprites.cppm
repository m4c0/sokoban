export module sprites;
import quack;

export namespace spr {
// Shifts are a reminder for using POT textures to avoid FP imprecisions in GPU
constexpr const unsigned cols = 1 << 4;
constexpr const unsigned rows = 1 << 2;

constexpr const auto w = 1.0f / static_cast<float>(cols);
constexpr const auto h = 1.0f / static_cast<float>(rows);
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

unsigned level(quack::mapped_buffers &all, float x, float y) {
  auto &[c, m, p, u] = all;
  *p++ = {{x, y}, {2, 1}};
  *u++ = {{1_w, 0_h}, {3_w, 1_h}};
  *c++ = {0, 0, 0, 0};
  *m++ = {1, 1, 1, 1};
  return 1;
}
} // namespace spr::blit

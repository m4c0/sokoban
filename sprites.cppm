export module sprites;
import quack;

export namespace spr {
// Shifts are a reminder for using POT textures to avoid FP imprecisions in GPU
constexpr const unsigned cols = 1 << 2;
constexpr const unsigned rows = 1 << 2;

constexpr const auto w = 1.0f / static_cast<float>(cols);
constexpr const auto h = 1.0f / static_cast<float>(rows);
} // namespace spr

export namespace spr::blit {
unsigned digit(quack::mapped_buffers &all, unsigned d, float x, float y) {
  auto &[c, m, p, u] = all;
  auto uu = (d % 6) / 8.0f;
  auto uv = (d / 6) / 4.0f;

  *p++ = {{x, 0}, {0.5, 1}};
  *u++ = {{0.25f + uu, 0.25f + uv}, {0.375f + uu, 0.5f + uv}};
  *c++ = {0, 0, 0, 0};
  *m++ = {1, 1, 1, 1};
  return 1;
}
unsigned number(quack::mapped_buffers &all, unsigned n, float x, float y) {
  unsigned count{};
  for (auto i = 0; i < 2; i++) {
    count += digit(all, n % 10, x, 0);

    x -= 0.5;
    n /= 10;
  }
  return count;
}

unsigned level(quack::mapped_buffers &all, float x, float y) {
  auto &[c, m, p, u] = all;
  *p++ = {{x, y}, {2, 1}};
  *u++ = {{0.25, 0.0}, {0.75, 0.25}};
  *c++ = {0, 0, 0, 0};
  *m++ = {1, 1, 1, 1};
  return 1;
}
} // namespace spr::blit

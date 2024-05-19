export module sprites;

export namespace spr {
// Shifts are a reminder for using POT textures to avoid FP imprecisions in GPU
constexpr const unsigned cols = 1 << 2;
constexpr const unsigned rows = 1 << 2;
} // namespace spr

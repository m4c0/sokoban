#pragma leco app

import casein;
import quack;
import silog;
import stubby;
import traits;
import voo;

using namespace traits::ints;

// Shifts are a reminder for using POT textures to avoid FP imprecisions in GPU
static constexpr const unsigned image_w = 8 * (1 << 0);
static constexpr const unsigned image_h = 8 * (1 << 2);

static unsigned g_cursor_x{};
static unsigned g_cursor_y{};
static bool g_cursor_hl{};
static uint32_t g_pixies[image_h][image_w]{};

static void update_atlas(voo::h2l_image *img) {
  voo::mapmem m{img->host_memory()};
  auto *buf = static_cast<uint32_t *>(*m);
  for (auto y = 0; y < image_h; y++) {
    for (auto x = 0; x < image_w; x++, buf++) {
      *buf = g_pixies[y][x];
    }
  }
}

struct : public quack::donald {
  const char *app_name() const noexcept override { return "bited"; }
  unsigned max_quads() const noexcept override { return 2; }
  unsigned quad_count() const noexcept override { return max_quads(); }
  quack::upc push_constants() const noexcept override {
    quack::upc res{};
    res.grid_size = {image_w, image_h};
    res.grid_pos = res.grid_size * 0.5;
    return res;
  }

  atlas create_atlas(voo::device_and_queue *dq) override {
    return atlas::make(dq->queue(), &update_atlas, dq->physical_device(),
                       image_w, image_h);
  }
  void update_data(quack::mapped_buffers all) override {
    auto [c, m, p, u] = all;
    c[0] = {0, 0, 0, 1};
    m[0] = {1, 1, 1, 1};
    p[0] = {{0, 0}, {image_w, image_h}};
    u[0] = {{0, 0}, {1, 1}};

    if (g_cursor_hl) {
      c[1] = {0, 0, 0, 0};
    } else {
      c[1] = {1, 0, 0, 1};
    }
    m[1] = {1, 1, 1, 0};
    p[1] = {{static_cast<float>(g_cursor_x), static_cast<float>(g_cursor_y)},
            {1, 1}};
    u[1] = {};
  }
} r;

static void flip_cursor() {
  g_cursor_hl = !g_cursor_hl;
  r.refresh_batch();
}

static void down() {
  if (g_cursor_y >= image_h - 1)
    return;

  g_cursor_y++;
  r.refresh_batch();
}
static void up() {
  if (g_cursor_y == 0)
    return;

  g_cursor_y--;
  r.refresh_batch();
}
static void left() {
  if (g_cursor_x == 0)
    return;

  g_cursor_x--;
  r.refresh_batch();
}
static void right() {
  if (g_cursor_x >= image_w - 1)
    return;

  g_cursor_x++;
  r.refresh_batch();
}

static void flip() {
  auto &p = g_pixies[g_cursor_y][g_cursor_x];
  p = ~p;
  r.refresh_atlas();
}

static void save() {
  // TODO: improve safety with a hai::array<pixel> and X/Y accessors
  auto *pix = reinterpret_cast<stbi::pixel *>(g_pixies);
  stbi::write_rgba_unsafe("atlas.png", image_w, image_h, pix);
}

static constexpr bool sane_image_width(const stbi::image &img) {
  return img.width <= image_w;
}
static constexpr bool sane_image_height(const stbi::image &img) {
  return img.height <= image_h;
}
static constexpr bool sane_num_channels(const stbi::image &img) {
  return img.num_channels == 4;
}

struct init {
  init() {
    using namespace casein;

    handle(KEY_DOWN, K_DOWN, down);
    handle(KEY_DOWN, K_UP, up);
    handle(KEY_DOWN, K_LEFT, left);
    handle(KEY_DOWN, K_RIGHT, right);
    handle(KEY_DOWN, K_ENTER, save);
    handle(KEY_DOWN, K_SPACE, flip);

    handle(TIMER, &flip_cursor);

    stbi::load("atlas.png")
        .assert(sane_image_width, "image is wider than buffer")
        .assert(sane_image_height, "image is taller than buffer")
        .assert(sane_num_channels, "image is not RGBA")
        .map([](auto &&img) {
          auto *c = reinterpret_cast<unsigned char *>(g_pixies);
          for (auto i = 0; i < img.width * img.height * 4; i++) {
            c[i] = (*img.data)[i];
          }
        })
        .take([](auto err) {
          silog::log(silog::error, "failed to load atlas: %s", err);
        });
  }
} i;

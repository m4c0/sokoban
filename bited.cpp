#pragma leco app

import casein;
import quack;
import silog;
import stubby;
import traits;
import voo;

using namespace traits::ints;

// Shifts are a reminder for using POT textures to avoid FP imprecisions in GPU
static constexpr const unsigned cols = 1 << 2;
static constexpr const unsigned rows = 1 << 2;
static constexpr const unsigned image_w = 8 * cols;
static constexpr const unsigned image_h = 8 * rows;
static constexpr const unsigned quad_count = 1 + (rows * cols);

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

static quack::donald::atlas_t *bitmap(voo::device_and_queue *dq) {
  return new quack::donald::atlas_t{dq->queue(), &update_atlas,
                                    dq->physical_device(), image_w, image_h};
}

static unsigned update_data(quack::mapped_buffers all) {
  static constexpr const float inv_c = 1.0f / cols;
  static constexpr const float inv_r = 1.0f / rows;
  auto [c, m, p, u] = all;
  for (auto y = 0; y < rows; y++) {
    for (auto x = 0; x < cols; x++) {
      *c++ = {0, 0, 0, 1};
      *m++ = {1, 1, 1, 1};
      *p++ = {{x * 8.0f + 0.1f, y * 8.0f + 0.1f}, {8 - 0.2f, 8 - 0.2f}};
      *u++ = {{x * inv_c, y * inv_r}, {(x + 1) * inv_c, (y + 1) * inv_r}};
    }
  }

  if (g_cursor_hl) {
    *c++ = {0, 0, 0, 0};
  } else {
    *c++ = {1, 0, 0, 1};
  }
  *m++ = {1, 1, 1, 0};
  *p++ = {{static_cast<float>(g_cursor_x), static_cast<float>(g_cursor_y)},
          {1, 1}};
  *u++ = {};

  return quad_count;
}

void refresh_atlas() { quack::donald::atlas(bitmap); }
void refresh_batch() { quack::donald::data(update_data); }

static void flip_cursor() {
  g_cursor_hl = !g_cursor_hl;
  refresh_batch();
}

static void down() {
  if (g_cursor_y >= image_h - 1)
    return;

  g_cursor_y++;
  refresh_batch();
}
static void up() {
  if (g_cursor_y == 0)
    return;

  g_cursor_y--;
  refresh_batch();
}
static void left() {
  if (g_cursor_x == 0)
    return;

  g_cursor_x--;
  refresh_batch();
}
static void right() {
  if (g_cursor_x >= image_w - 1)
    return;

  g_cursor_x++;
  refresh_batch();
}

static void flip() {
  auto &p = g_pixies[g_cursor_y][g_cursor_x];
  p = ~p;
  refresh_atlas();
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
          auto *d = reinterpret_cast<uint32_t *>(*img.data);
          for (auto y = 0; y < img.height; y++) {
            for (auto x = 0; x < img.width; x++) {
              g_pixies[y][x] = *d++;
            }
          }
        })
        .take([](auto err) {
          silog::log(silog::error, "failed to load atlas: %s", err);
        });

    quack::upc upc{};
    upc.grid_size = {image_w, image_h};
    upc.grid_pos = upc.grid_size * 0.5;

    using namespace quack::donald;
    app_name("bited");
    max_quads(quad_count);
    push_constants(upc);
    refresh_atlas();
    refresh_batch();
  }
} i;

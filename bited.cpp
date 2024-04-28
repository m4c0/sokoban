#pragma leco app

import casein;
import fork;
import quack;
import voo;

static constexpr const unsigned image_w = 8;
static constexpr const unsigned image_h = 8 * 4;

static void update_atlas(voo::h2l_image *img) {}

static unsigned g_cursor_x{};
static unsigned g_cursor_y{};
static bool g_cursor_hl{};

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
      c[1] = {1, 1, 1, 1};
    }
    m[1] = {1, 1, 1, 1};
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

struct init {
  init() {
    using namespace casein;

    handle(KEY_DOWN, K_DOWN, down);
    handle(KEY_DOWN, K_UP, up);
    handle(KEY_DOWN, K_LEFT, left);
    handle(KEY_DOWN, K_RIGHT, right);

    handle(TIMER, &flip_cursor);
  }
} i;

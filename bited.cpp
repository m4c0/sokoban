#ifndef LECO_TARGET_IOS
#pragma leco app
#endif
#pragma leco add_shader "quack.vert"
#pragma leco add_shader "quack.frag"

import casein;
import dotz;
import fff;
import jojo;
import quack;
import silog;
import sires;
import stubby;
import traits;
import vapp;
import voo;

using namespace traits::ints;

static constexpr const unsigned cols = 1 << 6;
static constexpr const unsigned rows = 1 << 4;
static constexpr const unsigned image_w = 8 * cols;
static constexpr const unsigned image_h = 8 * rows;
static constexpr const unsigned quad_count = 1 + (rows * cols);

static unsigned g_cursor_x{};
static unsigned g_cursor_y{};
static bool g_cursor_hl{};
static uint32_t g_pixies[image_h][image_w]{};

struct upc {
  dotz::vec2 grid_pos{};
  dotz::vec2 grid_size{1, 1};
} g_pc;

struct rotation {
  float angle;
  float rel_x;
  float rel_y;
  float pad; // Currently unused
};
struct inst {
  dotz::vec2 position;
  dotz::vec2 size;
  dotz::vec2 uv0;
  dotz::vec2 uv1;
  dotz::vec4 colour;
  dotz::vec4 multiplier;
  rotation rotation;
};
static_assert(sizeof(inst) == 20 * sizeof(float));
static constexpr const unsigned buf_size = sizeof(inst) * quad_count;

static VkDeviceMemory g_mem;
static unsigned g_inst_count;

static VkImage g_img;

static void refresh_batch() {
  if (!g_mem) return;

  voo::memiter<inst> m { g_mem, &g_inst_count };

  static constexpr const float inv_c = 1.0f / cols;
  static constexpr const float inv_r = 1.0f / rows;
  for (auto y = 0; y < rows; y++) {
    for (auto x = 0; x < cols; x++) {
      m += inst {
          .position{x * 8.0f + 0.1f, y * 8.0f + 0.1f},
          .size{8 - 0.2f, 8 - 0.2f},
          .uv0{x * inv_c, y * inv_r},
          .uv1{(x + 1) * inv_c, (y + 1) * inv_r},
          .multiplier{1},
      };
    }
  }

  m += inst {
      .position{static_cast<float>(g_cursor_x), static_cast<float>(g_cursor_y)},
      .size{1},
      .colour = g_cursor_hl ? dotz::vec4{} : dotz::vec4{1, 0, 0, 1},
      .multiplier{1, 1, 1, 0},
  };
}

static void refresh_atlas() {
  auto img = stbi::load(jojo::slurp("atlas.png"));
  unsigned w = img.width;
  unsigned h = img.height;
  unsigned sz = w * h * 4;
  auto host = voo::bound_buffer::create_from_host(sz, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
  {
    voo::memiter<unsigned char> c { *host.memory };
    for (auto i = 0; i < sz; i++) c[i] = (*img.data)[i];
  }

  voo::copy_buffer_to_image_sync({ w, h }, *host.buffer, g_img);
}

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
  silog::log(silog::info, "Atlas saved");
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

struct init : vapp {
  fff::timer m_timer { 300, flip_cursor };

  init() {
    using namespace casein;

    handle(KEY_DOWN, K_DOWN, down);
    handle(KEY_DOWN, K_UP, up);
    handle(KEY_DOWN, K_LEFT, left);
    handle(KEY_DOWN, K_RIGHT, right);
    handle(KEY_DOWN, K_ENTER, save);
    handle(KEY_DOWN, K_SPACE, flip);
  }

  void run() override {
    auto file = jojo::slurp("atlas.png");
    auto img = stbi::load(file.begin(), file.size());
    silog::assert(sane_image_width(img),  "image is wider than buffer");
    silog::assert(sane_image_height(img), "image is taller than buffer");
    silog::assert(sane_num_channels(img), "image is not RGBA");
    auto *d = reinterpret_cast<uint32_t *>(*img.data);
    for (auto y = 0; y < img.height; y++) {
      for (auto x = 0; x < img.width; x++) {
        g_pixies[y][x] = *d++;
      }
    }

    g_pc = {
      .grid_pos { image_w * 0.5, image_h * 0.5 },
      .grid_size { image_w, image_h },
    };

    main_loop("bited", [&](auto & dq, auto & sw) {
      voo::one_quad oq {};

      auto dsl = vee::create_descriptor_set_layout({ vee::dsl_fragment_sampler() });
      auto dpool = vee::create_descriptor_pool(1, { vee::combined_image_sampler(1) });
      auto dset = vee::allocate_descriptor_set(*dpool, *dsl);

      auto pl = vee::create_pipeline_layout(*dsl, vee::vert_frag_push_constant_range<upc>());
      auto rp = voo::single_att_render_pass(dq);
      auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = *rp,
        .shaders {
          *voo::vert_shader("quack.vert.spv"),
          *voo::frag_shader("quack.frag.spv"),
        },
        .bindings {
          oq.vertex_input_bind(),
          vee::vertex_input_bind_per_instance(sizeof(inst)),
        },
        .attributes {
          oq.vertex_attribute(0),
          vee::vertex_attribute_vec4(1, 0),
          vee::vertex_attribute_vec4(1, 8 * sizeof(float)),
          vee::vertex_attribute_vec4(1, 4 * sizeof(float)),
          vee::vertex_attribute_vec4(1, 12 * sizeof(float)),
          vee::vertex_attribute_vec4(1, 16 * sizeof(float)),
        },
      });

      auto buf = voo::bound_buffer::create_from_host(buf_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
      g_mem = *buf.memory;
      refresh_batch();

      auto smp = vee::create_sampler(vee::sampler_create_info {}.linear());

      auto img = voo::bound_image::create({ image_w, image_h }, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
      vee::update_descriptor_set(dset, 0, *img.iv, *smp);
      g_img = *img.img;
      refresh_atlas();

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit([&] {
          auto crp = sw.cmd_render_pass();
          auto cb = sw.command_buffer();
          vee::cmd_bind_gr_pipeline(cb, *gp);
          vee::cmd_push_vert_frag_constants(cb, *pl, &g_pc);
          vee::cmd_set_viewport(cb, sw.extent());
          vee::cmd_set_scissor(cb, sw.extent());
          vee::cmd_bind_vertex_buffers(cb, 1, *buf.buffer);
          oq.run(cb, 0, g_inst_count);
        });
      });
    });
  }
} i;

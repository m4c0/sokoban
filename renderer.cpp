#pragma leco add_resource "atlas.png"
#pragma leco add_shader "sokoban.vert"
#pragma leco add_shader "sokoban.frag"
module game;
import dotz;
import quack;
import silog;
import sitime;
import sprites;
import traits;
import vee;
import voo;

using namespace traits::ints;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

namespace {
  struct upc {
    dotz::vec4 sel_rect;
    dotz::vec2 player_pos;
    dotz::vec2 label_pos;
    dotz::vec2 menu_size;
    float level;
    float aspect;
    float time;
  };
  struct map_t {
    uint8_t type;
    uint8_t pad[3];
  };

  static hai::fn<void, quack::instance *&> g_updater;
  static quack::buffer_updater * g_buffer;
  static voo::updater<voo::h2l_image> * g_map;
  static upc g_pc {};

  static void updater(quack::instance *& i) { g_updater(i); }

  static void map_updater(voo::h2l_image * img) {
    auto mm = voo::mapmem { img->host_memory() };
    auto ptr = static_cast<map_t *>(*mm);

    for (auto i = 0U; i < sl::level_quad_count(); i++) {
      auto b = sg::grid[i];
      int x = i % sl::level_width;
      int y = i / sl::level_width;
      ptr[y * 32 + x].type = b;
    }
  }

struct main : voo::casein_thread {
  void run() override {
    voo::device_and_queue dq { "sokoban" };

    quack::pipeline_stuff ps { dq, 2 };
    quack::buffer_updater u { &dq, sg::max_quads, &updater };
    quack::image_updater a { &dq, &ps, voo::load_sires_image("atlas.png") };
    voo::updater<voo::h2l_image> map { dq.queue(), voo::h2l_image { dq.physical_device(), 32, 32, vee::image_format_rgba_uint }, &map_updater };

    g_buffer = &u;
    g_map = &map;

    map.run_once();

    vee::descriptor_set_layout dsl = vee::create_descriptor_set_layout({ vee::dsl_fragment_sampler(), vee::dsl_fragment_sampler() });
    vee::pipeline_layout pl = vee::create_pipeline_layout({ *dsl }, { vee::vert_frag_push_constant_range<upc>() });
    voo::one_quad_render oqr { "sokoban", &dq, *pl };

    auto n_smp = vee::create_sampler(vee::nearest_sampler);
    auto l_smp = vee::create_sampler(vee::linear_sampler);

    auto ds_pool = vee::create_descriptor_pool(1, { vee::combined_image_sampler(2) });
    auto dset = vee::allocate_descriptor_set(*ds_pool, *dsl);
    vee::update_descriptor_set(dset, 0, map.data().iv(), *n_smp);
    vee::update_descriptor_set(dset, 1, a.data().iv(), *l_smp);

    quack::upc rpc{};
    rpc.grid_size = {sl::level_width, sl::level_height};
    rpc.grid_pos = rpc.grid_size / 2.0;

    sitime::stopwatch t {};

    while (!interrupted()) {
      voo::swapchain_and_stuff sw { dq };

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          g_pc.player_pos.x = sg::player_pos % sl::level_width;
          g_pc.player_pos.y = sg::player_pos / sl::level_width;
          g_pc.level = sl::current_level() + 1.0f;
          g_pc.aspect = sw.aspect();
          g_pc.time = t.millis() / 1000.0f;

          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_bind_descriptor_set(*scb, *pl, 0, dset);
          vee::cmd_push_vert_frag_constants(*scb, *pl, &g_pc);
          oqr.run(*scb, sw.extent());

          quack::run(&ps, {
            .sw = &sw,
            .scb = *scb,
            .pc = &rpc,
            .inst_buffer = u.data().local_buffer(),
            .atlas_dset = a.dset(),
            .count = u.count(),
          });
        });
      });
    }
  }
} i;
} // namespace

static auto find_label_x() {
  for (auto x = 0; x < sl::level_width; x++) {
    for (auto y = 0; y < sl::level_height; y++) {
      auto p = y * sl::level_width + x;
      if (sg::grid[p] != spr::outside) return x;
    }
  }
  // Should not happen
  return 0;
}
static auto find_label_y() {
  for (auto y = 0; y < sl::level_height; y++) {
    for (auto x = 0; x < sl::level_width; x++) {
      auto p = y * sl::level_width + x;
      if (sg::grid[p] != spr::outside) return y - 1;
    }
  }
  // Should not happen
  return 0;
}

void sr::update_data(quack::instance *& all, dotz::vec2 menu_sz, dotz::vec4 sel) {
  float draw_y = find_label_y();
  float draw_x = find_label_x();
  g_pc.label_pos = { draw_x, draw_y };
  g_pc.menu_size = menu_sz;
  g_pc.sel_rect = sel;
}

void sr::set_updater(hai::fn<void, quack::instance *&> u) {
  g_updater = u;
  if (g_buffer) g_buffer->run_once();
  if (g_map) g_map->run_once();
}

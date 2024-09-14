#pragma leco add_resource "atlas.png"
#pragma leco add_shader "sokoban.vert"
#pragma leco add_shader "sokoban.frag"
module game;
import casein;
import quack;
import silog;
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
    float aspect;
  };
  struct map_t {
    uint8_t type;
    uint8_t pad[3];
  };

  static hai::fn<void, quack::instance *&> g_updater;
  static quack::buffer_updater * g_buffer;
  static voo::updater<voo::h2l_image> * g_map;

  static void updater(quack::instance *& i) { g_updater(i); }

  static void map_updater(voo::h2l_image * img) {
    auto mm = voo::mapmem { img->host_memory() };
    auto ptr = static_cast<map_t *>(*mm);

    for (auto i = 0U; i < sl::level_quad_count(); i++) {
      auto b = sg::grid[i];
      if (sg::player_pos == i) {
        b = (b == spr::target) ? spr::player_target : spr::player;
      }
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
    voo::one_quad quad { dq };

    g_buffer = &u;
    g_map = &map;

    map.run_once();

    vee::descriptor_set_layout dsl = vee::create_descriptor_set_layout({ vee::dsl_fragment_sampler() });
    vee::pipeline_layout pl = vee::create_pipeline_layout({ *dsl }, { vee::vert_frag_push_constant_range<upc>() });
    auto gp = vee::create_graphics_pipeline({
        .pipeline_layout = *pl,
        .render_pass = dq.render_pass(),
        .shaders {
            voo::shader("sokoban.vert.spv").pipeline_vert_stage(),
            voo::shader("sokoban.frag.spv").pipeline_frag_stage(),
        },
        .bindings { quad.vertex_input_bind() },
        .attributes { quad.vertex_attribute(0) },
    });

    auto smp = vee::create_sampler(vee::nearest_sampler);

    auto ds_pool = vee::create_descriptor_pool(1, { vee::combined_image_sampler(1) });
    auto dset = vee::allocate_descriptor_set(*ds_pool, *dsl);
    vee::update_descriptor_set(dset, 0, map.data().iv(), *smp);

    quack::upc rpc{};
    rpc.grid_size = {sl::level_width, sl::level_height};
    rpc.grid_pos = rpc.grid_size / 2.0;

    while (!interrupted()) {
      voo::swapchain_and_stuff sw { dq };

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          upc pc {
            .aspect = sw.aspect(),
          };

          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_set_viewport(*scb, sw.extent());
          vee::cmd_set_scissor(*scb, sw.extent());
          vee::cmd_bind_gr_pipeline(*scb, *gp);
          vee::cmd_bind_descriptor_set(*scb, *pl, 0, dset);
          vee::cmd_push_vert_frag_constants(*scb, *pl, &pc);
          quad.run(scb, 0, 1);

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

void sr::update_data(quack::instance *& all) {
  for (auto i = 0U; i < sl::level_quad_count(); i++) {
    auto b = sg::grid[i];
    if (sg::player_pos == i) {
      b = (b == spr::target) ? spr::player_target : spr::player;
    }
    float x = i % sl::level_width;
    float y = i / sl::level_width;
    if (b == spr::wall) continue;
    spr::blit::block(all, x, y, b);
  }

  spr::blit::level(all, 0, 0);
  spr::blit::number(all, sl::current_level() + 1, 3.5, 0);
}

void sr::set_updater(hai::fn<void, quack::instance *&> u) {
  g_updater = u;
  if (g_buffer) g_buffer->run_once();
  if (g_map) g_map->run_once();
}

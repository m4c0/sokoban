#pragma leco add_resource "atlas.png"
#pragma leco add_shader "sokoban.vert"
#pragma leco add_shader "sokoban.frag"
module game;
import casein;
import quack;
import sprites;
import vee;
import voo;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

namespace {
  struct upc {
    float aspect;
  };

  static hai::fn<void, quack::instance *&> g_updater;
  static quack::buffer_updater * g_buffer;

  static void updater(quack::instance *& i) { g_updater(i); }

struct main : voo::casein_thread {
  void run() override {
    voo::device_and_queue dq { "sokoban" };

    quack::pipeline_stuff ps { dq, 2 };
    quack::buffer_updater u { &dq, sg::max_quads, &updater };
    quack::image_updater a { &dq, &ps, voo::load_sires_image("atlas.png") };
    voo::one_quad quad { dq };

    g_buffer = &u;

    vee::pipeline_layout pl = vee::create_pipeline_layout({ vee::vert_frag_push_constant_range<upc>() });
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
    spr::blit::block(all, x, y, b);
  }

  spr::blit::level(all, 0, 0);
  spr::blit::number(all, sl::current_level() + 1, 3.5, 0);
}

void sr::set_updater(hai::fn<void, quack::instance *&> u) {
  g_updater = u;
  if (g_buffer) g_buffer->run_once();
}

#pragma leco add_resource "atlas.png"
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
  static hai::fn<void, quack::instance *&> g_updater;
  static quack::buffer_updater * g_buffer;

  static void updater(quack::instance *& i) { g_updater(i); }

struct main : voo::casein_thread {
  void run() override {
    voo::device_and_queue dq { "sokoban" };

    quack::pipeline_stuff ps { dq, 2 };
    quack::buffer_updater u { &dq, sg::max_quads, &updater };
    quack::image_updater a { &dq, &ps, voo::load_sires_image("atlas.png") };

    g_buffer = &u;

    quack::upc rpc{};
    rpc.grid_size = {sl::level_width, sl::level_height};
    rpc.grid_pos = rpc.grid_size / 2.0;

    while (!interrupted()) {
      voo::swapchain_and_stuff sw { dq };

      extent_loop(dq.queue(), sw, [&] {
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          auto scb = sw.cmd_render_pass(pcb);
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

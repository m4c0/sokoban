#ifndef LECO_TARGET_IOS
#pragma leco app
#endif

import casein;
import game;
import vee;
import voo;

namespace sl = sokoban::levels;
namespace sr = sokoban::renderer;

static void set_level(int dl) {
  auto ll = (sl::max_levels() + sl::current_level() + dl) % sl::max_levels();

  sl::load_level(sl::level(ll), ll);

  sr::set_updater([](auto *& i) -> void {
    sr::update_data(i, {}, {}, false, false);
  });
}

static void render_at(unsigned w, unsigned h) {
  vee::extent ext { w, h };
  auto pd = sr::g_dq->physical_device();
 
  // Colour buffer
  vee::image t_img = vee::create_renderable_image(ext);
  vee::device_memory t_mem = vee::create_local_image_memory(pd, *t_img);
  vee::bind_image_memory(*t_img, *t_mem);
  vee::image_view t_iv = vee::create_srgba_image_view(*t_img);

  // Depth buffer
  vee::image d_img = vee::create_depth_image(ext);
  vee::device_memory d_mem = vee::create_local_image_memory(pd, *d_img);
  vee::bind_image_memory(*d_img, *d_mem);
  vee::image_view d_iv = vee::create_depth_image_view(*d_img);

  // Host-readable output buffer
  vee::buffer o_buf = vee::create_transfer_dst_buffer(ext.width * ext.height * 4);
  vee::device_memory o_mem = vee::create_host_buffer_memory(pd, *o_buf);
  vee::bind_buffer_memory(*o_buf, *o_mem);

  // Render pass + Framebuffer + Pipeline
  vee::render_pass rp = vee::create_render_pass(pd, nullptr);
  vee::framebuffer fb = vee::create_framebuffer({
      .physical_device = pd,
      .render_pass = *rp,
      .image_buffer = *t_iv,
      .depth_buffer = *d_iv,
      .extent = ext,
  });

  auto cp = vee::create_command_pool(sr::g_dq->queue_family());
  auto cb = vee::allocate_primary_command_buffer(*cp);

  voo::cmd_buf_one_time_submit pcb{cb};
  vee::cmd_begin_render_pass({
      .command_buffer = cb,
      .render_pass = sr::g_dq->render_pass(),
      .framebuffer = *fb,
      .extent = ext,
      .clear_color = {{0.01, 0.02, 0.05, 1.0}},
      .use_secondary_cmd_buf = false,
  });

  sr::g_dq->queue()->queue_submit({ .command_buffer = cb });
  sr::g_dq->queue()->device_wait_idle();
}

static void render_shots() {
  render_at(1024, 768);
  render_at(800, 600);
}

struct init {
  init() {
    using namespace casein;
    handle(KEY_DOWN, K_LEFT, [] { set_level(-1); });
    handle(KEY_DOWN, K_RIGHT, [] { set_level(1); });
    handle(KEY_DOWN, K_ENTER, render_shots);

    sl::read_levels();
    set_level(0);
  }
} i;

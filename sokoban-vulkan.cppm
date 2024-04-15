#pragma leco add_shader "sokoban.vert"
#pragma leco add_shader "sokoban.frag"

export module sokoban:vulkan;
import :atlas;
import :grid;
import casein;
import vee;
import voo;

struct v2 {
  float v[2]{};
};
struct v4 {
  float v[4]{};
};

struct upc {
  v2 grid_pos;
  v2 grid_size;
};

struct inst {
  v4 colour;
  v4 uv;
};

class renderer : public voo::casein_thread {
  static constexpr auto uv(atlas_sprites s) {
    constexpr const auto h = 1.0f / static_cast<float>(sprite_count);
    const auto n = static_cast<unsigned>(s);
    return v4{0, n * h, 1, (n + 1) * h};
  }
  static constexpr auto uv(char b) {
    switch (b) {
    case box:
    case target_box:
      return uv(sprite_box);
    case target:
      return uv(sprite_target);
    case player:
    case player_target:
      return uv(sprite_player);
    default:
      return uv(sprite_empty);
    }
  }
  static constexpr auto colour(char b) {
    switch (b) {
    case target_box:
    case target:
    case player_target:
      return v4{0, 0.7, 0, 1};
    case wall:
      return v4{0, 0, 1, 1};
    case outside:
      return v4{};
    case box:
    case player:
    case empty:
      return v4{0, 0.3, 0, 1};
    }
    return v4{1, 0, 1, 1};
  }

  void load_atlas(voo::h2l_image &img) {
    constexpr const auto atl = atlas();
    auto m = img.mapmem();
    auto *d = static_cast<unsigned char *>(*m);
    for (auto r8 : atl.data)
      *d++ = r8;
  }
  void load_positions(voo::h2l_buffer &buf) {
    auto m = buf.mapmem();
    auto *d = static_cast<v2 *>(*m);

    // TODO: use instance ID
    unsigned i = 0;
    for (float y = 0; y < level_height; y++) {
      for (float x = 0; x < level_width; x++, i++) {
        d[i] = {x, y};
      }
    }
  }

  voo::h2l_buffer *m_insts;
  volatile float m_aspect{1};

public:
  void run() override {
    constexpr const auto sprite_count = level_width * level_height;

    voo::device_and_queue dq{"sokoban", native_ptr()};

    voo::one_quad quad{dq};
    voo::h2l_image img{dq, atlas_col_count, atlas_row_count, false};
    voo::h2l_buffer pos_buf{dq, sprite_count * sizeof(v2)};
    voo::h2l_buffer inst_buf{dq, sprite_count * sizeof(inst)};

    auto cb = vee::allocate_primary_command_buffer(dq.command_pool());

    auto dsl = vee::create_descriptor_set_layout({vee::dsl_fragment_sampler()});
    auto dpool =
        vee::create_descriptor_pool(1, {vee::combined_image_sampler(2)});
    auto dset = vee::allocate_descriptor_set(*dpool, *dsl);

    auto smp = vee::create_sampler(vee::nearest_sampler);
    vee::update_descriptor_set(dset, 0, img.iv(), *smp);

    auto pl = vee::create_pipeline_layout(
        {*dsl}, {vee::vertex_push_constant_range<upc>()});

    m_insts = &inst_buf;
    release_init_lock();
    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      auto gp = vee::create_graphics_pipeline({
          .pipeline_layout = *pl,
          .render_pass = sw.render_pass(),
          .shaders{
              voo::shader("sokoban.vert.spv").pipeline_vert_stage(),
              voo::shader("sokoban.frag.spv").pipeline_frag_stage(),
          },
          .bindings{
              quad.vertex_input_bind(),
              vee::vertex_input_bind_per_instance(sizeof(v2)),
              vee::vertex_input_bind_per_instance(sizeof(inst)),
          },
          .attributes{
              quad.vertex_attribute(0),
              vee::vertex_attribute_vec2(1, 0),
              vee::vertex_attribute_vec4(2, 0),
              vee::vertex_attribute_vec4(2, sizeof(v4)),
          },
      });

      load_atlas(img);
      load_positions(pos_buf);

      upc upc{
          .grid_pos = {level_width / 2, level_height / 2},
      };
      extent_loop(dq, sw, [&] {
        pos_buf.submit(dq);
        inst_buf.submit(dq);
        img.submit(dq);

        constexpr const float gw = level_width / 2.0;
        constexpr const float gh = level_height / 2.0;
        constexpr const float grid_aspect = gw / gh;

        upc.grid_size = grid_aspect < m_aspect ? v2{m_aspect * gh, gh}
                                               : v2{gw, gw / m_aspect};

        sw.one_time_submit(dq, cb, [&](auto &pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          auto *pc = &upc;

          vee::cmd_bind_gr_pipeline(*scb, *gp);
          vee::cmd_push_vertex_constants(*scb, *pl, pc);
          vee::cmd_bind_descriptor_set(*scb, *pl, 0, dset);
          vee::cmd_bind_vertex_buffers(*scb, 1, pos_buf.buffer());
          vee::cmd_bind_vertex_buffers(*scb, 2, inst_buf.buffer());
          quad.run(*scb, 0, sprite_count);
        });
      });
    }
  }

  void render(const grid &g, unsigned p) {
    auto lck = wait_init();

    auto m = m_insts->mapmem();
    auto is = static_cast<inst *>(*m);
    auto i = 0U;
    for (char b : g) {
      if (p == i) {
        b = (b == target) ? player_target : player;
      }
      *is++ = inst{
          .colour = colour(b),
          .uv = uv(b),
      };
      i++;
    }
  }

  void resize_window(const casein::events::resize_window &e) override {
    casein_thread::resize_window(e);

    float sw = (*e).width;
    float sh = (*e).height;
    m_aspect = sw / sh;
  }

  static auto &instance() noexcept {
    static renderer r{};
    return r;
  }
};

module sokoban;
import :atlas;
import :grid;
import casein;
import quack;
import vee;
import voo;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;

class updater : public quack::instance_batch_thread {
  // {{{ quad map utils
  static constexpr auto uv(atlas_sprites s) {
    constexpr const auto h = 1.0f / static_cast<float>(sprite_count);
    const auto n = static_cast<unsigned>(s);
    return quack::uv{{0, n * h}, {1, (n + 1) * h}};
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
      return quack::colour{0, 0.7, 0, 1};
    case wall:
      return quack::colour{0, 0, 1, 1};
    case outside:
      return quack::colour{};
    case box:
    case player:
    case empty:
      return quack::colour{0, 0.3, 0, 1};
    }
    return quack::colour{1, 0, 1, 1};
  }
  // }}}

  void map_all(all all) override {
    auto [c, m, p, u] = all;
    // {{{ quad memory map
    auto i = 0U;
    for (char b : sg::grid) {
      if (sg::player_pos == i) {
        b = (b == target) ? player_target : player;
      }
      float x = i % sl::level_width;
      float y = i / sl::level_width;
      *p++ = {{x, y}, {1, 1}};
      *u++ = uv(b);
      *c++ = colour(b);
      *m++ = quack::colour{1, 1, 1, 1};
      i++;
    }
    // }}}
  }

public:
  // {{{ max quads
  const unsigned max_quads() { return sl::level_width * sl::level_height; }
  // }}}

  explicit updater(voo::device_and_queue *dq, quack::pipeline_stuff &ps)
      : instance_batch_thread{dq->queue(), ps.create_batch(max_quads())} {}
};

static volatile bool dirty{true};
static struct : public voo::casein_thread {
  void run() override {
    // {{{ app name
    const auto app_name = "sokoban";
    // }}}

    voo::device_and_queue dq{app_name, native_ptr()};
    quack::pipeline_stuff ps{dq, 1};
    updater u{&dq, ps};

    atlas_img a{&dq};
    a.run_once();

    auto smp = vee::create_sampler(vee::nearest_sampler);
    auto dset = ps.allocate_descriptor_set(a.data().iv(), *smp);

    quack::upc rpc{};
    // {{{ grid size/pos
    rpc.grid_size = {sl::level_width, sl::level_height};
    rpc.grid_pos = rpc.grid_size / 2.0;
    // }}}

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      extent_loop(dq.queue(), sw, [&] {
        if (dirty) {
          u.run_once();
          dirty = false;
        }

        auto upc = quack::adjust_aspect(rpc, sw.aspect());
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          // {{{ quad count
          auto quad_count = sl::level_width * sl::level_height;
          // }}}
          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_set_viewport(*scb, sw.extent());
          vee::cmd_set_scissor(*scb, sw.extent());
          u.batch().build_commands(*pcb);
          ps.cmd_bind_descriptor_set(*scb, dset);
          ps.cmd_push_vert_frag_constants(*scb, upc);
          ps.run(*scb, quad_count);
        });
      });
    }
  }
} r;

void sokoban::renderer::render() { dirty = true; }
void sokoban::renderer::process_event(const casein::event &e) { r.handle(e); }

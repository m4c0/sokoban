module sokoban;
import :atlas;
import :grid;
import casein;
import quack;
import vee;
import voo;

namespace sg = sokoban::game;
namespace sl = sokoban::levels;

class updater : public voo::update_thread {
  quack::instance_batch m_ib;

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

  void build_cmd_buf(vee::command_buffer cb) override {
    m_ib.map_all([&](auto all) {
      auto [c, m, _, u] = all;
      auto i = 0U;
      for (char b : sg::grid) {
        if (sg::player_pos == i) {
          b = (b == target) ? player_target : player;
        }
        *u++ = uv(b);
        *c++ = colour(b);
        *m++ = quack::colour{1, 1, 1, 1};
        i++;
      }
    });

    voo::cmd_buf_one_time_submit pcb{cb};
    m_ib.setup_copy(cb);
  }

public:
  explicit updater(voo::device_and_queue *dq, quack::pipeline_stuff &ps)
      : update_thread{dq->queue()}
      , m_ib{ps.create_batch(sl::level_width * sl::level_height)} {
    m_ib.map_positions([](auto *is) {
      unsigned i = 0;
      for (float y = 0; y < sl::level_height; y++) {
        for (float x = 0; x < sl::level_width; x++, i++) {
          is[i] = {{x, y}, {1, 1}};
        }
      }
    });
  }

  [[nodiscard]] constexpr auto &batch() noexcept { return m_ib; }

  using update_thread::run_once;
};

static volatile bool dirty{true};
static struct : public voo::casein_thread {
  void run() override {
    voo::device_and_queue dq{"sokoban", native_ptr()};
    quack::pipeline_stuff ps{dq, 1};
    updater u{&dq, ps};

    atlas_img a{&dq};
    a.run_once();

    auto smp = vee::create_sampler(vee::nearest_sampler);
    auto dset = ps.allocate_descriptor_set(a.iv(), *smp);

    quack::upc rpc{};
    rpc.grid_size = {sl::level_width, sl::level_height};
    rpc.grid_pos = rpc.grid_size / 2.0;

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      extent_loop(dq.queue(), sw, [&] {
        if (dirty) {
          u.run_once();
          dirty = false;
        }

        auto upc = quack::adjust_aspect(rpc, sw.aspect());
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_set_viewport(*scb, sw.extent());
          vee::cmd_set_scissor(*scb, sw.extent());
          u.batch().build_commands(*pcb);
          ps.cmd_bind_descriptor_set(*scb, dset);
          ps.cmd_push_vert_frag_constants(*scb, upc);
          ps.run(*scb, sl::level_width * sl::level_height);
        });
      });
    }
  }
} r;

void sokoban::renderer::render() { dirty = true; }
void sokoban::renderer::process_event(const casein::event &e) { r.handle(e); }

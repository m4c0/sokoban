export module sokoban:quack;
import :atlas;
import :grid;
import :levels;
import casein;
import quack;
import vee;
import voo;

class atlas_img : public voo::update_thread {
  voo::h2l_image m_img;

  void build_cmd_buf(vee::command_buffer cb) override {
    voo::cmd_buf_one_time_submit pcb{cb};
    m_img.setup_copy(cb);
  }

public:
  atlas_img(voo::device_and_queue *dq)
      : update_thread{dq->queue()}
      , m_img{dq->physical_device(), atlas_col_count, atlas_row_count} {
    constexpr const auto atl = atlas();

    voo::mapmem m{m_img.host_memory()};
    auto *rgba = static_cast<quack::u8_rgba *>(*m);
    for (auto r8 : atl.data) {
      *rgba++ = {r8, r8, r8, r8};
    }
  }

  [[nodiscard]] constexpr auto iv() const noexcept { return m_img.iv(); }

  using update_thread::run_once;
};

class updater : public voo::update_thread {
  static constexpr auto max_quads = level_width * level_height;

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
      for (char b : grid::instance()) {
        if (grid::instance().player_pos() == i) {
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
      , m_ib{ps.create_batch(max_quads)} {
    m_ib.map_positions([](auto *is) {
      unsigned i = 0;
      for (float y = 0; y < level_height; y++) {
        for (float x = 0; x < level_width; x++, i++) {
          is[i] = {{x, y}, {1, 1}};
        }
      }
    });
  }

  [[nodiscard]] constexpr auto &batch() noexcept { return m_ib; }

  using update_thread::run_once;
};

class renderer : public voo::casein_thread {
  volatile bool m_dirty{};

  renderer() = default;

public:
  void render() { m_dirty = true; };

  void run() override {
    voo::device_and_queue dq{"sokoban", native_ptr()};
    quack::pipeline_stuff ps{dq, 1};
    updater u{&dq, ps};

    atlas_img a{&dq};
    auto smp = vee::create_sampler(vee::nearest_sampler);
    auto dset = ps.allocate_descriptor_set(a.iv(), *smp);

    quack::upc rpc{};
    rpc.grid_size = {level_width, level_width};
    rpc.grid_pos = rpc.grid_size / 2.0;

    while (!interrupted()) {
      voo::swapchain_and_stuff sw{dq};

      extent_loop(dq.queue(), sw, [&] {
        if (m_dirty) {
          u.run_once();
          m_dirty = false;
        }

        auto upc = quack::adjust_aspect(rpc, sw.aspect());
        sw.queue_one_time_submit(dq.queue(), [&](auto pcb) {
          auto scb = sw.cmd_render_pass(pcb);
          vee::cmd_set_viewport(*scb, sw.extent());
          vee::cmd_set_scissor(*scb, sw.extent());
          u.batch().build_commands(*pcb);
          ps.cmd_bind_descriptor_set(*scb, dset);
          ps.cmd_push_vert_frag_constants(*scb, upc);
          ps.run(*scb, level_width * level_height);
        });
      });
    }
  }

  static auto &instance() noexcept {
    static renderer r{};
    return r;
  }
};

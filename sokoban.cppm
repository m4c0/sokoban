#pragma leco app

export module sokoban;
import :audio;
import :game;
import casein;
import quack;

class events : public casein::handler {
  events() = default;

public:
  void create_window(const casein::events::create_window &e) override {
    game_grid::instance().set_level(0);
  }
  void gesture(const casein::events::gesture &e) override {
    static constexpr const auto map = [] {
      casein::subevent_map<casein::events::gesture, casein::G_MAX> res{};
      res[casein::G_SWIPE_UP] = [](auto) { game_grid::instance().up(); };
      res[casein::G_SWIPE_DOWN] = [](auto) { game_grid::instance().down(); };
      res[casein::G_SWIPE_LEFT] = [](auto) { game_grid::instance().left(); };
      res[casein::G_SWIPE_RIGHT] = [](auto) { game_grid::instance().right(); };
      res[casein::G_SHAKE] = [](auto) { game_grid::instance().reset_level(); };
      return res;
    }();
    map.handle(e);
  }
  void key_down(const casein::events::key_down &e) override {
    static constexpr auto map = [] {
      casein::subevent_map<casein::events::key_down, casein::K_MAX> res{};
      res[casein::K_UP] = [](auto) { game_grid::instance().up(); };
      res[casein::K_DOWN] = [](auto) { game_grid::instance().down(); };
      res[casein::K_LEFT] = [](auto) { game_grid::instance().left(); };
      res[casein::K_RIGHT] = [](auto) { game_grid::instance().right(); };
      res[casein::K_SPACE] = [](auto) { game_grid::instance().reset_level(); };
      return res;
    }();
    map.handle(e);
  }

  static auto &instance() {
    static events i{};
    return i;
  }
};

extern "C" void casein_handle(const casein::event &e) {
  streamer::instance();
  quack::mouse_tracker::instance().handle(e);
  renderer::instance().handle(e);
  events::instance().handle(e);
}

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
    static const auto map = [] {
      casein::gesture_map res{&game_grid::instance()};
      res[casein::G_SWIPE_UP] = &game_grid::up;
      res[casein::G_SWIPE_DOWN] = &game_grid::down;
      res[casein::G_SWIPE_LEFT] = &game_grid::left;
      res[casein::G_SWIPE_RIGHT] = &game_grid::right;
      res[casein::G_SHAKE] = &game_grid::reset_level;
      return res;
    }();
    map.handle(e);
  }
  void key_down(const casein::events::key_down &e) override {
    static const auto map = [] {
      casein::key_down_map res{&game_grid::instance()};
      res[casein::K_UP] = &game_grid::up;
      res[casein::K_DOWN] = &game_grid::down;
      res[casein::K_LEFT] = &game_grid::left;
      res[casein::K_RIGHT] = &game_grid::right;
      res[casein::K_SPACE] = &game_grid::reset_level;
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

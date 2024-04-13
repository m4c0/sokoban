#pragma leco app

export module sokoban;
import :audio;
import :game;
import casein;
import quack;

class events : public casein::handler {
  game_grid *m_game;

public:
  constexpr explicit events(game_grid *g) : m_game{g} {}

  void create_window(const casein::events::create_window &e) override {
    m_game->set_level(0);
  }
  void gesture(const casein::events::gesture &e) override {
    const auto map = [this] {
      casein::gesture_map res{m_game};
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
    const auto map = [this] {
      casein::key_down_map res{m_game};
      res[casein::K_UP] = &game_grid::up;
      res[casein::K_DOWN] = &game_grid::down;
      res[casein::K_LEFT] = &game_grid::left;
      res[casein::K_RIGHT] = &game_grid::right;
      res[casein::K_SPACE] = &game_grid::reset_level;
      return res;
    }();
    map.handle(e);
  }
};

extern "C" void casein_handle(const casein::event &e) {
  static game_grid game{};
  static events evt{&game};

  streamer::instance();
  quack::mouse_tracker::instance().handle(e);
  renderer::instance().handle(e);
  evt.handle(e);
}

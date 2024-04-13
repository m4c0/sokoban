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
    switch (*e) {
    case casein::G_SWIPE_UP:
      m_game->up();
      break;
    case casein::G_SWIPE_DOWN:
      m_game->down();
      break;
    case casein::G_SWIPE_LEFT:
      m_game->left();
      break;
    case casein::G_SWIPE_RIGHT:
      m_game->right();
      break;
    case casein::G_SHAKE:
      m_game->reset_level();
      break;
    default:
      break;
    }
  }
  void key_down(const casein::events::key_down &e) override {
    switch (*e) {
    case casein::K_UP:
      m_game->up();
      break;
    case casein::K_DOWN:
      m_game->down();
      break;
    case casein::K_LEFT:
      m_game->left();
      break;
    case casein::K_RIGHT:
      m_game->right();
      break;
    case casein::K_SPACE:
      m_game->reset_level();
      break;
    default:
      break;
    }
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

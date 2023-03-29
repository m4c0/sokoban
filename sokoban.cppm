export module sokoban;
import :game;
import casein;
import quack;

extern "C" void casein_handle(const casein::event &e) {
  static game_grid r{};

  r.process_event(e);

  switch (e.type()) {
  case casein::CREATE_WINDOW:
    r.set_level(0);
    break;
  case casein::KEY_DOWN:
    switch (*e.as<casein::events::key_down>()) {
    case casein::K_UP:
      r.up();
      break;
    case casein::K_DOWN:
      r.down();
      break;
    case casein::K_LEFT:
      r.left();
      break;
    case casein::K_RIGHT:
      r.right();
      break;
    case casein::K_SPACE:
      r.reset_level();
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

#pragma leco app
export module sokoban;
import casein;
import game;

extern "C" void casein_handle(const casein::event &e) {
  sokoban::renderer::process_event(e);
  sokoban::game_grid::process_event(e);
}

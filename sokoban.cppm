#pragma leco app
export module sokoban;
import game;

struct init {
  init() { sokoban::game_grid::init(); }
} i;

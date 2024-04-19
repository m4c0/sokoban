#pragma leco app
export module sokoban;
import game;

static struct init {
  init() { sokoban::game_grid::init(); }
} i;

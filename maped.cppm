#pragma leco app
export module maped;
import game;

static struct init {
  init() { sokoban::game_grid::set_level(2); }
} i;

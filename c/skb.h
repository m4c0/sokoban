#ifndef SKB_H
#define SKB_H

typedef struct {
  void (*escape)();
  void (*space)();
  void (*move)(int dx, int dy);
} skb_api_t;

extern const skb_api_t * skb_api;

void skb_init();

#ifdef SKB_IMPL
#include "gme.h"
#include "lvl.h"
#include "vlk-sokoban.h"

const skb_api_t * skb_api;

static void skb_nil() {}
static void skb_game();
const skb_api_t skb_api_menu = {
  .escape = &skb_game,
  .space  = &skb_nil,
  .move   = &skb_nil,
};

static void skb_main_menu() {
  vlk_menu_size(4, 3);
  skb_api = &skb_api_menu;
}
static void skb_reset() {
  lvl_load(lvl_current, gme_map);
  vlk_update_map();
}
static void skb_move(int dx, int dy) {
  gme_move(dx, dy);
  vlk_update_map();
}
const skb_api_t skb_api_game = {
  .escape = &skb_main_menu,
  .space  = &skb_reset,
  .move   = &skb_move,
};

static void skb_game() {
  vlk_menu_size(0, 0);
  skb_api = &skb_api_game;
}

void skb_init() {
  skb_api = &skb_api_game;
}

#endif
#endif

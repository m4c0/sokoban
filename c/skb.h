#ifndef SKB_H
#define SKB_H

typedef struct {
  void (*reset)();
  void (*move)(int dx, int dy);
} skb_api_t;

extern const skb_api_t * skb_api;

void skb_init();

#ifdef SKB_IMPL
#include "gme.h"
#include "lvl.h"
#include "vlk-sokoban.h"

void skb_reset() {
  lvl_load(lvl_current, gme_map);
  vlk_update_map();
}
void skb_move(int dx, int dy) {
  gme_move(dx, dy);
  vlk_update_map();
}
const skb_api_t skb_api_game = {
  .reset = &skb_reset,
  .move  = &skb_move,
};

const skb_api_t * skb_api;

void skb_init() {
  skb_api = &skb_api_game;
}

#endif
#endif

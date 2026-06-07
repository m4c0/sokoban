#ifndef SKB_H
#define SKB_H

typedef struct {
  void (*escape)();
  void (*space)();
  void (*move)(int dx, int dy);

  void (*mouse_move)(int dx, int dy);
  void (*mouse_down)(int dx, int dy);
  void (*mouse_up)(int dx, int dy);
} skb_api_t;

extern const skb_api_t * skb_api;

void skb_init();

#ifdef SKB_IMPL
#include "gme.h"
#include "lvl.h"
#include "mui.h"
#include "vlk-sokoban.h"

const skb_api_t * skb_api;

static void skb_game();
static void skb_reset();

static void skb_nil() {}

static void skb_mui_mouse_move(int x, int y) {
  mu_input_mousemove(&mui_ctx, x, y);
}
static void skb_mui_mouse_down(int x, int y) {
  mu_input_mousedown(&mui_ctx, x, y, 1);
}
static void skb_mui_mouse_up(int x, int y) {
  mu_input_mouseup(&mui_ctx, x, y, 1);
}

const skb_api_t skb_api_menu = {
  .escape     = &skb_game,
  .space      = &skb_nil,
  .move       = &skb_nil,
  .mouse_move = &skb_mui_mouse_move,
  .mouse_down = &skb_mui_mouse_down,
  .mouse_up   = &skb_mui_mouse_up,
};

static void skb_main_menu() {
  vlk_overlay(1);
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
  .escape     = &skb_main_menu,
  .space      = &skb_reset,
  .move       = &skb_move,
  .mouse_move = &skb_mui_mouse_move,
  .mouse_down = &skb_mui_mouse_down,
  .mouse_up   = &skb_mui_mouse_up,
};

static void skb_game() {
  vlk_overlay(0);
  skb_api = &skb_api_game;
}

void skb_init() {
  skb_api = &skb_api_game;
}

#endif
#endif

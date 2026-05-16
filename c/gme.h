#ifndef GME_H
#define GME_H

typedef struct gme_vec2 {
  float x, y;
} gme_vec2_t;
typedef struct gme_vec4 {
  float x, y, z, w;
} gme_vec4_t;
typedef struct gme_upc {
  gme_vec4_t sel_rect;
  gme_vec2_t player_pos;
  gme_vec2_t label_pos;
  gme_vec2_t menu_size;
  float level;
  float aspect;
  float time;
  float back_btn_dim;
  float menu_btn_dim;
} gme_upc_t;

enum gme_blocks {
  gme_b_player        = 'P',
  gme_b_player_target = 'p',
  gme_b_wall          = 'X',
  gme_b_empty         = '.',
  gme_b_outside       = ' ',
  gme_b_target        = '*',
  gme_b_box           = 'O',
  gme_b_target_box    = '0',
};

extern gme_upc_t gme_pc;

#ifdef GME_IMPL
#include "spr.h"

#define GME_WIDTH  32
#define GME_HEIGHT 24

gme_upc_t gme_pc;

#endif
#endif

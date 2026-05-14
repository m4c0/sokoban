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

extern gme_upc_t gme_pc;

#ifdef GME_IMPL

gme_upc_t gme_pc;

#endif
#endif

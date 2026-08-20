#ifndef GLU_H
#define GLU_H

//#include "gme.h"
#include "lvl.h"
//#include "mui.h"
#include "sfx.h"

#define GLU_BUF_SIZE (LVL_SZ * 4)

typedef struct glu_upc_s {
  float sel_rect_x, sel_rect_y, sel_rect_w, sel_rect_h;
  float player_pos_x, player_pos_y;
  float label_pos_x, label_pos_y;
  float cursor_x, cursor_y;
  float level;
  float aspect;
  float time;
  float overlay;
  float back_btn_dim;
  float menu_btn_dim;
} glu_upc_t;
glu_upc_t glu_pc;

typedef struct glu_init_s {
  const void * level;
  unsigned level_sz;

  int sound;
  int scr_w, scr_h;
} glu_init_t;

void glu_resize(unsigned w, unsigned h) {
}
void glu_init(const glu_init_t * t) {
  lvl_init(t->level, t->level_sz);

  sfx_init(t->sound);
  glu_resize(t->scr_w, t->scr_h);
}

void glu_deinit(void) {
}

void glu_load(void * into) {
}
void glu_frame(void) {
}

void glu_move(int dx, int dy) {
  // gme_move(dx, dy);
}

void glu_mouse_move(int x, int y) {
  // mu_input_mousemove(&mui_ctx, dx, dy);
}
void glu_mouse_down(int x, int y) {
  // mu_input_mousedown(&mui_ctx, dx, dy, 1);
}
void glu_mouse_up(int x, int y) {
  // mu_input_mouseup(&mui_ctx, dx, dy, 1);
}

#endif


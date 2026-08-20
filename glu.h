#ifndef GLU_H
#define GLU_H

//#include "gme.h"
#include "lvl.h"
//#include "mui.h"
#include "sfx.h"

#define GLU_BUF_SIZE (LVL_SZ * 4)

typedef struct glu_upc_s {
} glu_upc_t;

glu_upc_t glu_pc;

void glu_resize(unsigned w, unsigned h) {
}
void glu_init(int sound, unsigned w, unsigned h) {
  sfx_init(sound);
  glu_resize(w, h);
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


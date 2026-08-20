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
void glu_load(void * into) {
}
void glu_frame(void) {
}

#endif


#ifndef SFX_H
#define SFX_H

void sfx_filler(float * buf, unsigned sz);

void sfx_drag();
void sfx_eol();
void sfx_fail();
void sfx_target();

#ifdef SFX_IMPL

void sfx_filler(float * buf, unsigned sz) {
  for (int i = 0; i < sz; i++) {
    buf[i] = 0;
  }
}

void sfx_drag() {
  // 200
}
void sfx_eol() {
  // 50
}
void sfx_fail() {
  // 150
}
void sfx_target() {
  // 100
}

#endif
#endif

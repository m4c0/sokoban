#ifndef SFX_H
#define SFX_H

extern int sfx_enabled;

void sfx_filler(float * buf, unsigned sz);

void sfx_drag();
void sfx_eol();
void sfx_fail();
void sfx_target();

#ifdef SFX_IMPL

int sfx_enabled = 1;

static unsigned sp = 0;
static unsigned d = 1;

void sfx_filler(float * buf, unsigned sz) {
  int ssp = sp;
  float mult;
  if (ssp < 1000) {
    mult = ssp / 1000.0f;
  } else if (ssp < 4000) {
    mult = 1.0;
  } else if (ssp < 5000) {
    mult = (5000 - ssp) / 1000.0f;
  } else {
    mult = 0;
  }
  for (unsigned i = 0; i < sz; ++i) {
    buf[i] = 0.25f * mult * ((ssp / d) % 2) - 0.5f;
    ssp++;
  }
  sp = ssp;
}

static void sfx_play(unsigned div) {
  if (!sfx_enabled) return;

  d = div;
  sp = 0;
}
void sfx_drag()   { sfx_play(200); }
void sfx_eol()    { sfx_play( 50); }
void sfx_fail()   { sfx_play(150); }
void sfx_target() { sfx_play(100); }

#endif
#endif

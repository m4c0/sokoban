#ifndef GME_H
#define GME_H

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

extern char * gme_map;

#ifdef GME_IMPL

char * gme_map;

#endif
#endif

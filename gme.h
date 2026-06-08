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

void gme_level(int l);
void gme_move(int dx, int dy);

#ifdef GME_IMPL
#include "lvl.h"
#include "sav.h"
#include "sfx.h"
#include "vlk-sokoban.h"

char * gme_map;

void gme_level(int l) {
  if (l < 60) lvl_current = l;

  lvl_load(lvl_current, gme_map);
  vlk_update_map();

  sav_data.cur_level = lvl_current;
  if (sav_data.cur_level >= sav_data.max_level) sav_data.max_level = sav_data.cur_level + 1;
  sav_save();
}

void gme_move(int dx, int dy) {
  int px = lvl_px + dx;
  int py = lvl_py + dy;
  int p = py * LVL_WIDTH + px;

  int bx = px + dx;
  int by = py + dy;
  int b = by * LVL_WIDTH + bx;

  switch (gme_map[p]) {
    case gme_b_outside:
    case gme_b_wall:
      return sfx_fail();
    case gme_b_empty:
    case gme_b_target:
    case gme_b_player:
    case gme_b_player_target:
      lvl_px = px; lvl_py = py;
      break;
    case gme_b_box:
    case gme_b_target_box:
      switch (gme_map[b]) {
        int open_tgts;
        case gme_b_player:
        case gme_b_player_target:
          gme_map[b] = (gme_map[b] == gme_b_player) ? gme_b_empty : gme_b_target;
        case gme_b_empty:
        case gme_b_target:
          lvl_px = px; lvl_py = py;
          gme_map[p] = (gme_map[p] == gme_b_target_box) ? gme_b_target : gme_b_empty;
          gme_map[b] = (gme_map[b] == gme_b_target) ? gme_b_target_box : gme_b_box;

          open_tgts = 0;
          for (int i = 0; i < LVL_SZ; i++) {
            if (gme_map[i] == gme_b_target) open_tgts++;
          }
          if (open_tgts == 0) {
            sfx_eol();
            gme_level(lvl_current + 1);
          } else if (gme_map[b] == gme_b_target_box) {
            sfx_target();
          } else {
            sfx_drag();
          }
          break;
        default:
          return sfx_fail();
      }
      break;
  }
}

#endif
#endif

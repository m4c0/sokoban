#ifndef LVL_H
#define LVL_H

#define LVL_WIDTH  32
#define LVL_HEIGHT 24
#define LVL_SZ (32 * 24)

extern int lvl_current;
extern int lvl_px, lvl_py;
extern int lvl_min_x, lvl_min_y;
extern int lvl_max_level;

extern FILE * lvl_f;

void lvl_init(FILE * f);
void lvl_load(int n, char * buffer);

#ifdef LVL_IMPL

FILE * lvl_f;
int lvl_current;
int lvl_px, lvl_py;
int lvl_min_x, lvl_min_y;
int lvl_max_level;

void lvl_init(FILE * f) {
  lvl_f = f;
  lvl_max_level = -1;

  assert(0 == fseek(lvl_f, 0, SEEK_SET));

  char buf[LVL_SZ * 2];
  while (!feof(f)) {
    if (!fgets(buf, sizeof(buf), lvl_f)) break;
    lvl_max_level++;
  }
  assert(lvl_max_level >= 0);
}
void lvl_load(int n, char * buffer) {
  assert(0 == fseek(lvl_f, 0, SEEK_SET));

  for (int i = 0; i <= n; i++) {
    assert('|' == fgetc(lvl_f));
    assert(1 == fread(buffer, LVL_SZ, 1, lvl_f));
    assert('|' == fgetc(lvl_f));

    char nl[2];
    assert(fgets(nl, 2, lvl_f));
  }

  lvl_current = n;
  lvl_min_x = lvl_min_y = 0;
  lvl_px = lvl_py = 0;

  for (int y = 0; y < LVL_HEIGHT; y++) {
    for (int x = 0; x < LVL_WIDTH; x++) {
      int p = y * LVL_WIDTH + x;
      switch (buffer[p]) {
        case 'X':
          if (lvl_min_y == 0) lvl_min_y = y;
          if (lvl_min_x == 0) lvl_min_x = x;
          if (lvl_min_x > x) lvl_min_x = x;
          break;
        case 'P':
          lvl_px = x;
          lvl_py = y;
          break;
      }
    }
  }
  assert(lvl_px && lvl_py);
}

#endif
#endif

#ifndef LVL_H
#define LVL_H

#define LVL_WIDTH  32
#define LVL_HEIGHT 24

extern int lvl_current;
extern int lvl_px;
extern int lvl_py;

void lvl_load(FILE * f, int n, char * buffer);

#ifdef LVL_IMPL
#define LVL_SZ (32 * 24)

int lvl_current;
int lvl_px;
int lvl_py;

void lvl_load(FILE * f, int n, char * buffer) {
  for (int i = 0; i <= n; i++) {
    assert('|' == fgetc(f));
    assert(1 == fread(buffer, LVL_SZ, 1, f));
    assert('|' == fgetc(f));

    char nl[2];
    assert(fgets(nl, 2, f));
  }
  fclose(f);
  lvl_current = n;

  for (int y = 0; y < LVL_HEIGHT; y++) {
    for (int x = 0; x < LVL_WIDTH; x++) {
      int p = y * LVL_WIDTH + x;
      switch (buffer[p]) {
        case 'P':
          lvl_px = x;
          lvl_py = y;
          break;
      }
    }
  }
}

#endif
#endif

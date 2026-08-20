#ifndef LVL_H
#define LVL_H

#define LVL_WIDTH  32
#define LVL_HEIGHT 24
#define LVL_SZ (32 * 24)

extern int lvl_current;
extern int lvl_px, lvl_py;
extern int lvl_min_x, lvl_min_y;
extern int lvl_max_level;

void lvl_init(const void * data, int sz);
void lvl_load(int n, char * buffer);

#ifdef LVL_IMPL

const char * lvl_data;
const char * lvl_data_end;
int lvl_current;
int lvl_px, lvl_py;
int lvl_min_x, lvl_min_y;
int lvl_max_level;

void lvl_init(const void * data, int sz) {
  lvl_data = data;
  lvl_data_end = lvl_data + sz;
  lvl_max_level = -1;

  const char * ptr = lvl_data;
  while (ptr && ptr < lvl_data_end && *ptr) {
    if (*ptr == '\n') lvl_max_level++;
    ptr++;
  }
  assert(lvl_max_level >= 0);
}
void lvl_load(int n, char * buffer) {
  const char * ptr = lvl_data;
  for (int i = 0; i <= n; i++) {
    assert('|' == ptr[0]);
    assert('|' == ptr[LVL_SZ + 1]);
    assert('\n' == ptr[LVL_SZ + 2]);

    memcpy(buffer, ptr + 1, LVL_SZ);
    ptr += LVL_SZ + 3;
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

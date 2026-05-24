#ifndef LVL_H
#define LVL_H

#define LVL_WIDTH  32
#define LVL_HEIGHT 24

extern int lvl_current;

void lvl_load(FILE * f, int n, char * buffer);

#ifdef LVL_IMPL
#define LVL_SZ (32 * 24)

int lvl_current;

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
}

#endif
#endif

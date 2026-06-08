#ifndef SAV_H
#define SAV_H

typedef struct sav_s {
  unsigned cur_level;
  unsigned max_level;
} sav_t;

void sav_load(sav_t * v);
void sav_save(const sav_t * v);

#ifdef SAV_IMPL
#ifdef _WIN32
#  include <direct.h>
#  define SEP "\\"
#else
#  include <sys/stat.h>
#  define SEP "/"
#  define _mkdir(X) mkdir(X, 0777)
#endif

void sav_get_path(char * buf, unsigned sz);

static FILE * sav_open(const char * mode) {
  char path[10240];
  sav_get_path(path, sizeof(path));

  strlcat(path, SEP       , sizeof(path));
  strlcat(path, "sokoban" , sizeof(path));
  _mkdir(path);

  strlcat(path, SEP       , sizeof(path));
  strlcat(path, "save.dat", sizeof(path));
  return fopen(path, mode);
}

void sav_load(sav_t * v) {
  FILE * f = sav_open("rb");
  fread(v, sizeof(sav_t), 1, f);
  fclose(f);
}

void sav_save(const sav_t * v) {
  FILE * f = sav_open("wb");
  fwrite(v, sizeof(sav_t), 1, f);
  fclose(f);
}

#endif
#endif

module;
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

export module save;
import buoy;
import silog;

export namespace save {
struct data {
  unsigned cur_level{};
  unsigned max_level{};
};

data read() {
  auto path = buoy::path("sokoban", "save.dat");
  
  data res {};

  FILE * f = fopen(path.begin(), "rb");
  if (1 != fread(&res, sizeof(data), 1, f)) {
    silog::error("Error reading save data");
    res = {};
  }
  fclose(f);

  return res;
}

void write(data d) {
  auto path = buoy::path("sokoban", "save.dat");

  FILE * f = fopen(path.begin(), "rb");
  if (1 != fwrite(&d, sizeof(data), 1, f)) {
    silog::error("Error writing save data");
  }
  fclose(f);
}
} // namespace save

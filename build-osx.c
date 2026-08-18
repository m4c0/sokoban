#define CFLAGS "-g"

#include "build.h"

static void print_key(FILE * f, const char * key) {}

static int pch() {
  char * args[] = {
    "clang", "-Wall", "-g", "-x", "c-header",
    "-IVulkan-Headers/include",
    "-D", "VK_USE_PLATFORM_METAL_EXT",
    "-D", "VLK_USE_VOLK",
    "-o", "pch.pch", "pch.h", 0 };
  return run(args);
}

static int bited_exe() {
  char * args[] = {
    "clang", "-Wall",
    "-o", APP".app/Contents/MacOS/bited", 
    "bited.o", "vlk-bited.o", "volk.o",
    0 };
  return run(args);
}

static int maped_exe() {
  char * args[] = {
    "clang", "-Wall",
    "-o", APP".app/Contents/MacOS/maped", 
    "lvl.o", "maped.o", "vlk-maped.o", "volk.o",
    0 };
  return run(args);
}

static int link_exe() {
  char * args[] = {
    "clang", "-Wall",
    "-o", APP".app/Contents/MacOS/sokoban", 
    "gme.o", "lvl.o", "mui.o", "sav.o", "sfx.o", "snd.o", "volk.o",
    "microui.o", "vlk-sokoban.o", "sokoban-osx.o",
    0 };
  return run(args);
}

int main(int argc, char ** argv) {
  mkdir(APP".app", 0777);
  mkdir(APP".app/Contents", 0777);
  mkdir(APP".app/Contents/MacOS", 0777);
  mkdir(APP".app/Contents/Resources", 0777);

  if (pch()) return 1;

  HDR("volk", "VOLK_IMPLEMENTATION");

  HDR("gme", "GME_IMPL");
  HDR("lvl", "LVL_IMPL");
  HDR("mui", "MUI_IMPL");
  HDR("sav", "SAV_IMPL");
  HDR("sfx", "SFX_IMPL");
  HDR("snd", "SND_IMPL");

  CC("microui");

  CM("sokoban-osx");
  HDR("vlk-sokoban", "VLK_IMPL");
  if (link_exe()) return 1;

  CM("bited");
  HDR("vlk-bited", "VLK_IMPL");
  if (bited_exe()) return 1;

  CM("maped");
  HDR("vlk-maped", "VLK_IMPL");
  if (maped_exe()) return 1;

  SHADER("bited.frag");
  SHADER("bited.vert");
  SHADER("mui-vlk.frag");
  SHADER("mui-vlk.vert");
  SHADER("sokoban.frag");
  SHADER("sokoban.vert");

  RUN("cp", "atlas.img",  APP".app/Contents/Resources/");
  RUN("cp", "levels.txt", APP".app/Contents/Resources/");

  // Tempsies until Metal
  RUN("cp", "libvulkan.dylib", APP".app/Contents/MacOS/");
  RUN("cp", "bited.frag.spv", APP".app/Contents/Resources/");
  RUN("cp", "bited.vert.spv", APP".app/Contents/Resources/");
  RUN("cp", "mui-vlk.frag.spv", APP".app/Contents/Resources/");
  RUN("cp", "mui-vlk.vert.spv", APP".app/Contents/Resources/");
  RUN("cp", "sokoban.frag.spv", APP".app/Contents/Resources/");
  RUN("cp", "sokoban.vert.spv", APP".app/Contents/Resources/");

  return 0;
}

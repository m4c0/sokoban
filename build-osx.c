#define CFLAGS "-g"

#include "build.h"

static void print_key(FILE * f, const char * key) {}

static int shader(const char * app, char * name) {
  char spv[1024]; snprintf(spv, 1024, "%s.app/Contents/Resources/%s.spv", app, name);
  char * args[] = { "glslang", "-V", name, "-o", spv, 0 };
  return run(args);
}

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
    "-framework", "AppKit",
    "-framework", "AudioToolbox",
    "-framework", "MetalKit",
    "-o", "bited.app/Contents/MacOS/bited", 
    "bited.o", "vlk-bited.o", "volk.o",
    0 };
  return run(args);
}

static int maped_exe() {
  char * args[] = {
    "clang", "-Wall",
    "-framework", "AppKit",
    "-framework", "AudioToolbox",
    "-framework", "MetalKit",
    "-o", "maped.app/Contents/MacOS/maped", 
    "lvl.o", "maped.o", "vlk-maped.o", "volk.o",
    0 };
  return run(args);
}

static int link_exe() {
  char * args[] = {
    "clang", "-Wall",
    "-framework", "AppKit",
    "-framework", "AudioToolbox",
    "-framework", "MetalKit",
    "-o", "sokoban.app/Contents/MacOS/sokoban", 
    "gme.o", "lvl.o", "mui.o", "sav.o", "sfx.o", "snd.o", "volk.o",
    "microui.o", "vlk-sokoban.o", "sokoban-osx.o",
    0 };
  return run(args);
}

static void mkd(const char * n, const char * p) {
  char buf[1024];
  snprintf(buf, 1024, "%s.app/%s", n, p);
  mkdir(buf, 0777);
}
static int app(const char * n) {
  mkd(n, "");
  mkd(n, "Contents");
  mkd(n, "Contents/MacOS");
  mkd(n, "Contents/Resources");

  char buf[1024];
  snprintf(buf, 1024, "%s.app/Contents/MacOS/", n);

  char * args[] = { "cp", "libvulkan.dylib", buf, 0 };
  return run(args);
}

static int cp_frag() {
  char * args[] = { "cp",
    "sokoban.app/Contents/Resources/sokoban.frag.spv",
    "maped.app/Contents/Resources/",
    0 };
  return run(args);
}
static int cp_vert() {
  char * args[] = { "cp",
    "sokoban.app/Contents/Resources/sokoban.vert.spv",
    "maped.app/Contents/Resources/",
    0 };
  return run(args);
}

int main(int argc, char ** argv) {
  if (pch()) return 1;

  HDR("volk", "VOLK_IMPLEMENTATION");

  HDR("gme", "GME_IMPL");
  HDR("lvl", "LVL_IMPL");
  HDR("mui", "MUI_IMPL");
  HDR("sav", "SAV_IMPL");
  HDR("sfx", "SFX_IMPL");
  HDR("snd", "SND_IMPL");

  CC("microui");

  if (app("sokoban")) return 1;
  CM("sokoban-osx");
  HDR("vlk-sokoban", "VLK_IMPL");
  if (link_exe()) return 1;

  if (app("bited")) return 1;
  CM("bited");
  HDR("vlk-bited", "VLK_IMPL");
  if (bited_exe()) return 1;

  if (app("maped")) return 1;
  CM("maped");
  HDR("vlk-maped", "VLK_IMPL");
  if (maped_exe()) return 1;

  if (shader("bited", "bited.frag")) return 1;
  if (shader("bited", "bited.vert")) return 1;
  if (shader("sokoban", "mui-vlk.frag")) return 1;
  if (shader("sokoban", "mui-vlk.vert")) return 1;
  if (shader("sokoban", "sokoban.frag")) return 1;
  if (shader("sokoban", "sokoban.vert")) return 1;
  if (cp_frag()) return 1;
  if (cp_vert()) return 1;

  RUN("cp", "atlas.img",  "maped.app/Contents/Resources/");
  RUN("cp", "atlas.img",  "sokoban.app/Contents/Resources/");
  RUN("cp", "levels.txt", "sokoban.app/Contents/Resources/");

  return 0;
}

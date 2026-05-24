#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <sys/stat.h>
#include <assert.h>
#include <direct.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>

#define OPT "-gdwarf"
//#define OPT "-O3"

static void usage() {
  fprintf(stderr, "just call 'build' without arguments\n");
}

static int run(char ** args) {
  assert(args && args[0]);

  if (0 == _spawnvp(_P_WAIT, args[0], (const char * const *)args)) {
    return 0;
  }

  fprintf(stderr, "failed to run child process: %s\n", args[0]);
  return 1;
}

static int shader(char * name) {
  char spv[1024]; snprintf(spv, 1024, "app/%s.spv", name);
  char src[1024]; snprintf(src, 1024, "../%s", name);

  char * args[] = { "glslang", "-V", src, "-o", spv, 0 };
  return run(args);
}

static int pch() {
  char * args[] = {
    "clang", "-Wall", OPT, "-x", "c-header",
    "-IVulkan-Headers/include",
    "-D", "VK_USE_PLATFORM_WIN32_KHR",
    "-D", "VLK_USE_VOLK",
    "-o", "pch.pch", "pch.h", 0 };
  return run(args);
}

static int cc(char * src, char * o) {
  char * args[] = {
    "clang", "-Wall", OPT, "-include-pch", "pch.pch",
    "-o", o, "-c", src, 0 };
  return run(args);
}

static int hdr(char * src, char * o, char * d) {
  char * args[] = {
    "clang", "-Wall", "-x", "c", OPT, "-include-pch", "pch.pch",
    "-D", d, "-o", o, "-c", src, 0
  };
  return run(args);
}

static int bited_exe() {
  char * args[] = {
    "clang", "-Wall", OPT,
    "-o", "app/bited.exe", 
    "bited.o", "vlk-bited.o", "volk.o",
    "-luser32",
    0 };
  return run(args);
}

static int maped_exe() {
  char * args[] = {
    "clang", "-Wall", OPT,
    "-o", "app/maped.exe", 
    "lvl.o", "maped.o", "vlk-maped.o", "volk.o",
    "-luser32",
    0 };
  return run(args);
}

static int link_exe() {
  char * args[] = {
    "clang", "-Wall", OPT,
    "-o", "app/sokoban.exe", 
    "gme.o", "spr.o", "volk.o",
    "vlk-sokoban.o", "vulkan-win.o",
    "-luser32",
    0 };
  return run(args);
}

int main(int argc, char ** argv) {
  if (argc != 1) return (usage(), 1);

  _mkdir("app");

  if (pch()) return 1;

  if (hdr("volk.h", "volk.o", "VOLK_IMPLEMENTATION")) return 1;

  if (hdr("gme.h", "gme.o", "GME_IMPL")) return 1;
  if (hdr("lvl.h", "lvl.o", "LVL_IMPL")) return 1;
  if (hdr("spr.h", "spr.o", "SPR_IMPL")) return 1;
  if (hdr("vlk-sokoban.h", "vlk-sokoban.o", "VLK_IMPL")) return 1;
  if (cc("vulkan-win.c", "vulkan-win.o")) return 1;
  if (link_exe()) return 1;

  if (hdr("vlk-bited.h", "vlk-bited.o", "VLK_IMPL")) return 1;
  if (cc("bited.c", "bited.o")) return 1;
  if (bited_exe()) return 1;

  if (hdr("vlk-maped.h", "vlk-maped.o", "VLK_IMPL")) return 1;
  if (cc("maped.c", "maped.o")) return 1;
  if (maped_exe()) return 1;

  if (shader("bited.frag")) return 1;
  if (shader("bited.vert")) return 1;
  if (shader("sokoban.frag")) return 1;
  if (shader("sokoban.vert")) return 1;

  return 0;
}

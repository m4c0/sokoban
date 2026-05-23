#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void usage() {
  fprintf(stderr, "just call 'build' without arguments\n");
}

static int run(char ** args) {
  assert(args && args[0]);

  pid_t pid = fork();
  if (pid == 0) {
    execvp(args[0], args);
    abort();
  } else if (pid > 0) {
    int sl = 0;
    assert(0 <= waitpid(pid, &sl, 0));
    if (WIFEXITED(sl)) return WEXITSTATUS(sl);
  }

  fprintf(stderr, "failed to run child process: %s\n", args[0]);
  return 1;
}

static int shader(const char * app, char * name) {
  char spv[1024]; snprintf(spv, 1024, "%s.app/Contents/Resources/%s.spv", app, name);
  char src[1024]; snprintf(src, 1024, "../%s", name);

  char * args[] = { "glslang", "-V", src, "-o", spv, 0 };
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

static int cc(char * src, char * o) {
  char * args[] = {
    "clang", "-Wall", "-g", "-include-pch", "pch.pch",
    "-o", o, "-c", src, 0 };
  return run(args);
}

static int cm(char * src, char * o) {
  // It's nearly mandatory to use "modules" with ObjC.
  // The compilation speed without it is abismal.
  char * args[] = {
    "clang", "-Wall", "-g", "-fmodules",
    "-o", o, "-c", src, 0 };
  return run(args);
}

static int hdr(char * src, char * o, char * d) {
  char * args[] = {
    "clang", "-Wall", "-x", "c", "-g", "-include-pch", "pch.pch",
    "-D", d, "-o", o, "-c", src, 0
  };
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
    "maped.o", "vlk-maped.o", "volk.o",
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
    "gme.o", "spr.o", "volk.o",
    "vlk-sokoban.o", "vulkan-osx.o",
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
  if (argc != 1) return (usage(), 1);

  if (pch()) return 1;

  if (hdr("volk.h", "volk.o", "VOLK_IMPLEMENTATION")) return 1;

  if (app("sokoban")) return 1;
  if (cm("vulkan-osx.m", "vulkan-osx.o")) return 1;
  if (hdr("gme.h", "gme.o", "GME_IMPL")) return 1;
  if (hdr("spr.h", "spr.o", "SPR_IMPL")) return 1;
  if (hdr("vlk-sokoban.h", "vlk-sokoban.o", "VLK_IMPL")) return 1;
  if (link_exe()) return 1;

  if (app("bited")) return 1;
  if (cm("bited.m", "bited.o")) return 1;
  if (hdr("vlk-bited.h", "vlk-bited.o", "VLK_IMPL")) return 1;
  if (bited_exe()) return 1;

  if (app("maped")) return 1;
  if (cm("maped.m", "maped.o")) return 1;
  if (hdr("vlk-maped.h", "vlk-maped.o", "VLK_IMPL")) return 1;
  if (maped_exe()) return 1;

  if (shader("bited", "bited.frag")) return 1;
  if (shader("bited", "bited.vert")) return 1;
  if (shader("sokoban", "sokoban.frag")) return 1;
  if (shader("sokoban", "sokoban.vert")) return 1;
  if (cp_frag()) return 1;
  if (cp_vert()) return 1;

  return 0;
}

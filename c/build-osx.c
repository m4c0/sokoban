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

static int atlas() {
  int w, h, comp;
  stbi_uc * data = stbi_load("../atlas.png", &w, &h, &comp, 1);
  assert(data);
  assert(w == 512);
  assert(h == 128);
  assert(comp == 4);

  FILE * out = fopen("sokoban.app/Contents/Resources/atlas.img", "wb");
  assert(out);
  for (int i = 0; i < w * h; i++, data += 4) assert(data[3] == fputc(data[3], out));
  fclose(out);

  return 0;
}

static int shader(const char * app, char * name) {
  char spv[1024]; snprintf(spv, 1024, "%s.app/Contents/Resources/%s.spv", app, name);
  char src[1024]; snprintf(src, 1024, "../%s", name);

  char * args[] = { "glslang", "-V", src, "-o", spv, 0 };
  return run(args);
}

static int cc(char * src, char * o) {
  char * args[] = {
    "clang", "-Wall", "-g",
    "-IVulkan-Headers/include",
    "-o", o, "-c", src, 0 };
  return run(args);
}

static int hdr(char * src, char * o, char * d) {
  char * args[] = {
    "clang", "-Wall", "-x", "c", "-g",
    "-IVulkan-Headers/include",
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
    "bited.o", "vlk-bited.o",
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
    "gme.o", "spr.o", "vlk.o",
    "vulkan-osx.o",
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

int main(int argc, char ** argv) {
  if (argc != 1) return (usage(), 1);

  if (app("sokoban")) return 1;
  if (cc("vulkan-osx.m", "vulkan-osx.o")) return 1;
  if (hdr("gme.h", "gme.o", "GME_IMPL")) return 1;
  if (hdr("spr.h", "spr.o", "SPR_IMPL")) return 1;
  if (hdr("vlk.h", "vlk.o", "VLK_IMPL")) return 1;
  if (link_exe()) return 1;
  if (shader("sokoban", "sokoban.frag")) return 1;
  if (shader("sokoban", "sokoban.vert")) return 1;

  if (app("bited")) return 1;
  if (cc("bited.m", "bited.o")) return 1;
  if (hdr("vlk-bited.h", "vlk-bited.o", "VLK_IMPL")) return 1;
  if (bited_exe()) return 1;
  if (shader("bited", "bited.frag")) return 1;
  if (shader("bited", "bited.vert")) return 1;

  if (atlas()) return 1;

  return 0;
}

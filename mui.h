#ifndef MUI_H
#define MUI_H
#include "microui.h"

extern mu_Context mui_ctx;

void mui_init();
void mui_run(unsigned sw, unsigned sh);

int mui_font_width(char c);
int mui_font_height();

#ifdef MUI_IMPL
#include "gme.h"
#include "lvl.h"
#include "vlk-sokoban.h"

mu_Context mui_ctx = {0};

int mui_font_width(char c) {
  if ((c | 0x20) == 'i') return 1;
  if ((c | 0x20) == 'm') return 5;
  if ((c | 0x20) == 'n') return 4;
  return 3;
}

int mui_font_height() {
  return 5;
}

static int font_width(mu_Font f, const char * txt, int len) {
  int w = 0;
  for (; *txt; txt++) w += mui_font_width(*txt) * 3 + 2;
  return w;
}
static int font_height(mu_Font f) {
  return mui_font_height() * 3;
}

void mui_init() {
  mu_init(&mui_ctx);

  mui_ctx.text_width  = &font_width;
  mui_ctx.text_height = &font_height;

  mui_ctx.style->colors[MU_COLOR_WINDOWBG] = mu_color(10,  30, 20, 255);
  mui_ctx.style->colors[MU_COLOR_BUTTON]   = mu_color(70, 120, 90, 255);
}

static void mui_label(const char * txt) {
  int pad = mui_ctx.style->padding;

  mui_ctx.style->padding = 0;
  mu_label(&mui_ctx, txt);
  mui_ctx.style->padding = pad;
}
static void mui_vspace(int n) {
  mu_layout_row(&mui_ctx, 1, (int[]) { -1 }, n);
  mu_layout_next(&mui_ctx);
}

static int mui_snd_on = 1;
static float mui_lvl = 1;

void mui_run(unsigned sw, unsigned sh) {
  mu_begin(&mui_ctx);

  mui_ctx.style->padding = 12;
  mui_ctx.style->spacing = 8;

  int toggle_options = 0;

  int opt = MU_OPT_NOCLOSE | MU_OPT_NOTITLE | MU_OPT_NOFRAME | MU_OPT_NOSCROLL;
  if (mu_begin_window_ex(&mui_ctx, "!main", mu_rect(0, 0, sw, 70), opt)) {
    mu_layout_row(&mui_ctx, 2, (int[]) { -56, -1 }, 48);
    mu_layout_next(&mui_ctx);
    if (mu_button_ex(&mui_ctx, "", 0xEE00, opt)) toggle_options = 1;
    mu_end_window(&mui_ctx);
  }

  if (toggle_options) {
    mu_Container * cnt = mu_get_container(&mui_ctx, "!options");
    cnt->open = 1 - cnt->open;
    vlk_overlay(cnt->open);
    mui_lvl = lvl_current + 1;
  }
  int wx = (sw - 300) / 2;
  int wy = (sh - 200) / 2;
  opt = MU_OPT_NOCLOSE | MU_OPT_NOTITLE | MU_OPT_CLOSED;
  if (mu_begin_window_ex(&mui_ctx, "!options", mu_rect(wx, wy, 300, 200), opt)) {
    mui_vspace(6);

    mu_layout_row(&mui_ctx, 3, (int[]) { -60, -1 }, 32);
    mui_label("Sound");
    if (mu_button(&mui_ctx, mui_snd_on ? "ON" : "")) {
      mui_snd_on = !mui_snd_on;
    }

    mui_vspace(12);

    mu_layout_row(&mui_ctx, 1, (int[]) { -1 }, 32);
    if (mu_slider_ex(&mui_ctx, &mui_lvl, 1, 60, 1, "Level %.0f", MU_OPT_ALIGNCENTER)) {
      lvl_current = mui_lvl - 1;
      lvl_load(lvl_current, gme_map);
      vlk_update_map();
    }

    mui_vspace(12);

    if (mu_button(&mui_ctx, "Restart level")) {
      lvl_load(lvl_current, gme_map);
      vlk_update_map();
    }

    mu_end_window(&mui_ctx);
  }

  mu_end(&mui_ctx);
}

#endif
#endif

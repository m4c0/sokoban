#ifndef MUI_H
#define MUI_H
#include "microui.h"

extern mu_Context mui_ctx;

void mui_init();

void mui_none(unsigned, unsigned);
void mui_options(unsigned sw, unsigned sh);

int mui_font_width(char c);
int mui_font_height();

#ifdef MUI_IMPL

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

void mui_none(unsigned sw, unsigned sh) {
  mu_begin(&mui_ctx);
  mu_end(&mui_ctx);
}

static void mui_label(const char * txt) {
  int pad = mui_ctx.style->padding;

  mui_ctx.style->padding = 0;
  mu_label(&mui_ctx, txt);
  mui_ctx.style->padding = pad;
}
static void mui_vspace() {
  mu_layout_row(&mui_ctx, 1, (int[]) { -1 }, 12);
  mu_layout_next(&mui_ctx);
}

static int mui_snd_on = 1;
void mui_options(unsigned sw, unsigned sh) {
  int wx = (sw - 300) / 2;
  int wy = (sh - 200) / 2;

  mui_ctx.style->padding = 12;
  mui_ctx.style->spacing = 8;

  mu_begin(&mui_ctx);
  int opt = MU_OPT_NOCLOSE | MU_OPT_NOTITLE;
  if (mu_begin_window_ex(&mui_ctx, "", mu_rect(wx, wy, 300, 200), opt)) {
    mu_layout_row(&mui_ctx, 3, (int[]) { -60, -1 }, 32);
    mui_label("Sound");
    if (mu_button(&mui_ctx, mui_snd_on ? "ON" : "")) {
      mui_snd_on = !mui_snd_on;
    }

    mui_vspace();

    mu_layout_row(&mui_ctx, 1, (int[]) { -1 }, 32);
    if (mu_button(&mui_ctx, "Level select")) {}

    mui_vspace();

    mu_layout_row(&mui_ctx, 1, (int[]) { -1 }, 32);
    if (mu_button(&mui_ctx, "Restart level")) {}

    mu_end_window(&mui_ctx);
  }
  mu_end(&mui_ctx);
}

#endif
#endif

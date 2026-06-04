#ifndef MUI_H
#define MUI_H
#include "microui.h"

extern mu_Context mui_ctx;

void mui_init();
void mui_run(unsigned sw, unsigned sh);

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

  mui_ctx.style->padding = 24;
  mui_ctx.style->spacing = 24;

  mui_ctx.style->colors[MU_COLOR_WINDOWBG] = mu_color(10,  30, 20, 255);
  mui_ctx.style->colors[MU_COLOR_BUTTON]   = mu_color(70, 120, 90, 255);
}
void mui_run(unsigned sw, unsigned sh) {
  int wx = (sw - 300) / 2;
  int wy = (sh - 200) / 2;

  mu_begin(&mui_ctx);
  int opt = MU_OPT_NOCLOSE | MU_OPT_NOTITLE;
  if (mu_begin_window_ex(&mui_ctx, "", mu_rect(wx, wy, 300, 200), opt)) {
    mu_layout_row(&mui_ctx, 1, (int[]) { -1 }, 32);

    if (mu_button(&mui_ctx, "Le button")) {
      mu_open_popup(&mui_ctx, "popup");
    }
    if (mu_button(&mui_ctx, "Le otro button")) {
      mu_open_popup(&mui_ctx, "popup");
    }
    if (mu_button(&mui_ctx, "Mas uno button")) {
      mu_open_popup(&mui_ctx, "popup");
    }

    if (mu_begin_popup(&mui_ctx, "popup")) {
      mu_label(&mui_ctx, "Le popup");
      mu_end_popup(&mui_ctx);
    }

    mu_end_window(&mui_ctx);
  }
  mu_end(&mui_ctx);
}

#endif
#endif

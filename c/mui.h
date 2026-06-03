#ifndef MUI_H
#define MUI_H
#include "microui.h"

extern mu_Context mui_ctx;

void mui_init();
void mui_run(unsigned sw, unsigned sh);

#ifdef MUI_IMPL

mu_Context mui_ctx = {0};

static int font_width(mu_Font f, const char * txt, int len) {
  return 14 * strlen(txt);
}
static int font_height(mu_Font f) {
  return 21;
}

void mui_init() {
  mu_init(&mui_ctx);
  mui_ctx.text_width  = &font_width;
  mui_ctx.text_height = &font_height;
}
void mui_run(unsigned sw, unsigned sh) {
  int wx = (sw - 300) / 2;
  int wy = (sh - 200) / 2;

  mui_ctx.style->padding = 8;
  mui_ctx.style->spacing = 8;
  mui_ctx.style->title_height = 32;

  mu_begin(&mui_ctx);
  int opt = MU_OPT_NOCLOSE;
  if (mu_begin_window_ex(&mui_ctx, "Window", mu_rect(wx, wy, 300, 200), opt)) {
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

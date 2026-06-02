#ifndef MUI_H
#define MUI_H

void mui_init();

#ifdef MUI_IMPL
#include "microui.h"

static mu_Context ctx = {0};

void mui_init() {
  mu_init(&ctx);
}

#endif
#endif

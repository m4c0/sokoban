#ifndef BITED_H
#define BITED_H

typedef struct btd_upc {
  int x, y;
} btd_upc_t;
static btd_upc_t btd_pc;

static void btd_init(int sw, int sh) {}
static void btd_cursor(int dx, int dy) {}
static void btd_toggle() {}
static void btd_load() {}
static void btd_save() {}

#endif

#ifndef WG_MENU_H
#define WG_MENU_H

#include <stdbool.h>

#include "wg/framebuffer.h"
#include "wg/params.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool open;
  int cursor;
} wg_menu;

bool wg_menu_key(wg_menu *m, wg_params *p, int key);

void wg_menu_render(const wg_menu *m, const wg_params *p, wg_framebuffer *fb);

#ifdef __cplusplus
}
#endif

#endif

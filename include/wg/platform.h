#ifndef WG_PLATFORM_H
#define WG_PLATFORM_H

#include <stdbool.h>

#include "wg/framebuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wg_platform wg_platform;

typedef enum {
  WG_EVENT_NONE = 0,
  WG_EVENT_QUIT,
  WG_EVENT_KEY_DOWN,
  WG_EVENT_RESIZE,
  WG_EVENT_REDRAW
} wg_event_type;

enum {
  WG_KEY_ESCAPE = 27,
  WG_KEY_F1 = 0x100,
  WG_KEY_F11 = WG_KEY_F1 + 10,
  WG_KEY_F12 = WG_KEY_F1 + 11
};

typedef struct {
  wg_event_type type;
  int key;
} wg_event;

wg_platform *wg_platform_create(const char *title, int w, int h);
void wg_platform_destroy(wg_platform *p);

bool wg_platform_wait_event(wg_platform *p, wg_event *ev);

void wg_platform_get_size(wg_platform *p, int *w, int *h);

bool wg_platform_present(wg_platform *p, const wg_framebuffer *fb);

void wg_platform_toggle_fullscreen(wg_platform *p);
void wg_platform_set_title(wg_platform *p, const char *title);

#ifdef __cplusplus
}
#endif

#endif

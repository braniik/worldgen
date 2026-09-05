#ifndef WG_FRAMEBUFFER_H
#define WG_FRAMEBUFFER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint32_t *px;
  int w, h;
} wg_framebuffer;

bool wg_fb_init(wg_framebuffer *fb, int w, int h);
bool wg_fb_resize(wg_framebuffer *fb, int w, int h);
void wg_fb_free(wg_framebuffer *fb);

void wg_fb_clear(wg_framebuffer *fb, uint32_t color);

void wg_fb_fill_rect(wg_framebuffer *fb, int x, int y, int w, int h,
                     uint32_t color);

#ifdef __cplusplus
}
#endif

#endif

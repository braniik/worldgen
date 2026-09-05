#include "wg/framebuffer.h"

#include <stdlib.h>

bool wg_fb_init(wg_framebuffer *fb, int w, int h) {
  fb->px = NULL;
  fb->w = fb->h = 0;
  return wg_fb_resize(fb, w, h);
}

bool wg_fb_resize(wg_framebuffer *fb, int w, int h) {
  if (w < 1)
    w = 1;
  if (h < 1)
    h = 1;

  uint32_t *px = realloc(fb->px, (size_t)w * (size_t)h * sizeof *px);
  if (!px)
    return false;

  fb->px = px;
  fb->w = w;
  fb->h = h;
  return true;
}

void wg_fb_free(wg_framebuffer *fb) {
  free(fb->px);
  fb->px = NULL;
  fb->w = fb->h = 0;
}

void wg_fb_clear(wg_framebuffer *fb, uint32_t color) {
  size_t n = (size_t)fb->w * (size_t)fb->h;
  for (size_t i = 0; i < n; i++)
    fb->px[i] = color;
}

void wg_fb_fill_rect(wg_framebuffer *fb, int x, int y, int w, int h,
                     uint32_t color) {
  int x0 = x < 0 ? 0 : x;
  int y0 = y < 0 ? 0 : y;
  int x1 = x + w > fb->w ? fb->w : x + w;
  int y1 = y + h > fb->h ? fb->h : y + h;

  for (int yy = y0; yy < y1; yy++) {
    uint32_t *row = fb->px + (size_t)yy * (size_t)fb->w;
    for (int xx = x0; xx < x1; xx++)
      row[xx] = color;
  }
}

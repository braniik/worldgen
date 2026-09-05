#include "wg/render.h"

#define WG_COLOR_BG 0x000000u

static const uint32_t tile_colors[WG_TILE_COUNT] = {
    [WG_TILE_AIR] = 0x262626u,
    [WG_TILE_STONE] = 0x7a7a7au,
};

void wg_render_world(const wg_world *world, wg_framebuffer *fb) {
  wg_fb_clear(fb, WG_COLOR_BG);

  int tile_px = fb->w / world->w;
  if (fb->h / world->h < tile_px)
    tile_px = fb->h / world->h;
  if (tile_px < 1)
    tile_px = 1;

  // Below 3px per tile there's no room for a border without the fill vanishing,
  // so drop it.
  int border = tile_px >= 3 ? 1 : 0;
  int inner = tile_px - border;

  int ox = (fb->w - tile_px * world->w) / 2;
  int oy = (fb->h - tile_px * world->h) / 2;

  for (int y = 0; y < world->h; y++) {
    int py = oy + y * tile_px + border;
    for (int x = 0; x < world->w; x++) {
      int px = ox + x * tile_px + border;
      uint32_t c = tile_colors[wg_world_get(world, x, y)];
      wg_fb_fill_rect(fb, px, py, inner, inner, c);
    }
  }
}

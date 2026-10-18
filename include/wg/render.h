#ifndef WG_RENDER_H
#define WG_RENDER_H

#include <stdbool.h>

#include "wg/framebuffer.h"
#include "wg/world.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WG_GRID_MIN_TILE_PX 6

void wg_render_world(const wg_world *world, wg_framebuffer *fb, bool grid);

#ifdef __cplusplus
}
#endif

#endif

#ifndef WG_RENDER_H
#define WG_RENDER_H

#include "wg/framebuffer.h"
#include "wg/world.h"

#ifdef __cplusplus
extern "C" {
#endif

void wg_render_world(const wg_world *world, wg_framebuffer *fb);

#ifdef __cplusplus
}
#endif

#endif

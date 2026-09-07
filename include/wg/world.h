#ifndef WG_WORLD_H
#define WG_WORLD_H

#include <stdint.h>

#include "wg/params.h"
#include "wg/tiles.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WG_WORLD_WIDTH 256
#define WG_WORLD_HEIGHT 256

typedef struct {
  uint32_t seed;
  int w, h;
  uint8_t tiles[WG_WORLD_WIDTH * WG_WORLD_HEIGHT];
} wg_world;

void wg_world_generate(wg_world *world, uint32_t seed, const wg_params *p);

static inline uint8_t wg_world_get(const wg_world *world, int x, int y) {
  return world->tiles[y * world->w + x];
}

static inline void wg_world_set(wg_world *world, int x, int y, uint8_t tile) {
  world->tiles[y * world->w + x] = tile;
}

#ifdef __cplusplus
}
#endif

#endif

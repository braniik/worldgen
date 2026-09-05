#include "wg/world.h"
#include "wg/rng.h"

#define WG_GEN_FILL 0.5f

void wg_world_generate(wg_world *world, uint32_t seed) {
  world->seed = seed;
  world->w = WG_WORLD_WIDTH;
  world->h = WG_WORLD_HEIGHT;

  for (int y = 0; y < world->h; y++) {
    for (int x = 0; x < world->w; x++) {
      uint8_t t =
          wg_hash2f(seed, x, y) < WG_GEN_FILL ? WG_TILE_STONE : WG_TILE_AIR;
      wg_world_set(world, x, y, t);
    }
  }
}

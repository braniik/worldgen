#include "wg/world.h"

#include <string.h>

#include "wg/noise.h"

static uint8_t band(float v, const float levels[WG_TILE_COUNT - 1]) {
  uint8_t t = 0;
  while (t < WG_TILE_COUNT - 1 && v >= levels[t])
    t++;
  return t;
}

static void mode_filter(wg_world *w) {
  static uint8_t out[WG_WORLD_WIDTH * WG_WORLD_HEIGHT];

  for (int y = 0; y < w->h; y++) {
    for (int x = 0; x < w->w; x++) {
      int count[WG_TILE_COUNT] = {0};
      for (int dy = -1; dy <= 1; dy++) {
        int yy = y + dy;
        if (yy < 0 || yy >= w->h)
          continue;
        for (int dx = -1; dx <= 1; dx++) {
          int xx = x + dx;
          if (xx < 0 || xx >= w->w)
            continue;
          count[wg_world_get(w, xx, yy)]++;
        }
      }
      uint8_t best = wg_world_get(w, x, y); // ties keep the cell
      for (int t = 0; t < WG_TILE_COUNT; t++)
        if (count[t] > count[best])
          best = (uint8_t)t;
      out[y * w->w + x] = best;
    }
  }
  memcpy(w->tiles, out, (size_t)w->w * (size_t)w->h);
}

void wg_world_generate(wg_world *world, uint32_t seed, const wg_params *p) {
  world->seed = seed;
  world->w = WG_WORLD_WIDTH;
  world->h = WG_WORLD_HEIGHT;

  float levels[WG_TILE_COUNT - 1];
  wg_params_levels(p, levels);

  for (int y = 0; y < world->h; y++) {
    for (int x = 0; x < world->w; x++) {
      float v = wg_fbm(seed, x * p->frequency, y * p->frequency,
                       (int)p->octaves, p->lacunarity, p->gain);
      wg_world_set(world, x, y, band(v, levels));
    }
  }

  for (int i = 0; i < (int)p->mode_passes; i++)
    mode_filter(world);
}

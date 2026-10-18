#include "wg/world.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "wg/noise.h"

#define WG_WORLD_TILES (WG_WORLD_WIDTH * WG_WORLD_HEIGHT)

static uint8_t band(float v, const float levels[WG_TILE_COUNT - 1]) {
  uint8_t t = 0;
  while (t < WG_TILE_COUNT - 1 && v >= levels[t])
    t++;
  return t;
}

static int cmp_float(const void *a, const void *b) {
  float x = *(const float *)a, y = *(const float *)b;
  return (x > y) - (x < y);
}

static void resolve_levels(wg_world *w, const float lines[WG_TILE_COUNT - 1]) {
  static float sorted[WG_WORLD_TILES];
  size_t n = (size_t)w->w * (size_t)w->h;
  memcpy(sorted, w->height, n * sizeof *sorted);
  qsort(sorted, n, sizeof *sorted, cmp_float);

  for (int i = 0; i < WG_TILE_COUNT - 1; i++) {
    float q = lines[i];
    if (q <= 0.0f)
      w->level[i] = -INFINITY;
    else if (q >= 1.0f)
      w->level[i] = INFINITY;
    else
      w->level[i] = sorted[(size_t)(q * (float)n + 0.5f)];
  }
}

static void mode_filter(wg_world *w) {
  static uint8_t out[WG_WORLD_TILES];

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

static void add_rims(wg_world *w) {
  for (int L = WG_TILE_COUNT - 1; L >= 2; L--) {
    for (int y = 0; y < w->h; y++) {
      for (int x = 0; x < w->w; x++) {
        if (wg_world_get(w, x, y) >= L - 1)
          continue;
        bool touches = false;
        for (int dy = -1; dy <= 1 && !touches; dy++) {
          int yy = y + dy;
          if (yy < 0 || yy >= w->h)
            continue;
          for (int dx = -1; dx <= 1 && !touches; dx++) {
            int xx = x + dx;
            if (xx >= 0 && xx < w->w && wg_world_get(w, xx, yy) >= L)
              touches = true;
          }
        }
        if (touches)
          wg_world_set(w, x, y, (uint8_t)(L - 1));
      }
    }
  }
}

void wg_world_generate(wg_world *world, uint32_t seed, const wg_params *p) {
  world->seed = seed;
  world->w = WG_WORLD_WIDTH;
  world->h = WG_WORLD_HEIGHT;

  for (int y = 0; y < world->h; y++) {
    for (int x = 0; x < world->w; x++) {
      world->height[y * world->w + x] =
          wg_fbm(seed, (float)x * p->frequency, (float)y * p->frequency,
                 (int)p->octaves, p->lacunarity, p->gain);
    }
  }

  float lines[WG_TILE_COUNT - 1];
  wg_params_lines(p, lines);
  resolve_levels(world, lines);

  for (int i = 0; i < world->w * world->h; i++)
    world->tiles[i] = band(world->height[i], world->level);

  for (int i = 0; i < (int)p->mode_passes; i++)
    mode_filter(world);

  add_rims(world);
}

// Headless checks for the generator. Links only against src/core,
//
//   make check            # 200 seeds
//   make check ARGS=2000  # more seeds
//
// Exits non-zero if any check fails.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "wg/rng.h"
#include "wg/world.h"

static wg_world a, b;
static int failures;

#define CHECK(cond, ...)                                                       \
  do {                                                                         \
    if (!(cond)) {                                                             \
      printf("  FAIL: " __VA_ARGS__);                                          \
      putchar('\n');                                                           \
      failures++;                                                              \
    }                                                                          \
  } while (0)

static void coverage(const wg_world *w, double out[WG_TILE_COUNT]) {
  int n = w->w * w->h, count[WG_TILE_COUNT] = {0};
  for (int i = 0; i < n; i++)
    count[w->tiles[i]]++;
  for (int t = 0; t < WG_TILE_COUNT; t++)
    out[t] = 100.0 * count[t] / n;
}

static int band_skips(const wg_world *w) {
  int skips = 0;
  for (int y = 0; y < w->h; y++)
    for (int x = 0; x < w->w; x++)
      for (int dy = 0; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
          if (dy == 0 && dx <= 0)
            continue; // each pair once
          int xx = x + dx, yy = y + dy;
          if (xx < 0 || xx >= w->w || yy >= w->h)
            continue;
          if (abs(wg_world_get(w, x, y) - wg_world_get(w, xx, yy)) >= 2)
            skips++;
        }
  return skips;
}

static void run(const char *name, wg_params p, int seeds, double tolerance) {
  float lines[WG_TILE_COUNT - 1];
  wg_params_lines(&p, lines);
  double target[WG_TILE_COUNT], prev = 0.0;
  for (int t = 0; t < WG_TILE_COUNT; t++) {
    double line = t < WG_TILE_COUNT - 1 ? 100.0 * lines[t] : 100.0;
    target[t] = line - prev;
    prev = line;
  }

  double worst = 0.0;
  long skips = 0;
  uint32_t seed = 0x5EED0001u;
  clock_t t0 = clock();
  for (int s = 0; s < seeds; s++, seed = wg_seed_next(seed)) {
    wg_world_generate(&a, seed, &p);
    double cov[WG_TILE_COUNT];
    coverage(&a, cov);
    for (int t = 0; t < WG_TILE_COUNT; t++)
      worst = fmax(worst, fabs(cov[t] - target[t]));
    skips += band_skips(&a);
  }
  double ms = 1000.0 * (double)(clock() - t0) / CLOCKS_PER_SEC / seeds;

  printf("%-26s %4d seeds  worst coverage error %.2f%%  band skips %ld  "
         "(%.1f ms/world incl. checks)\n",
         name, seeds, worst, skips, ms);
  CHECK(worst <= tolerance, "coverage off by %.2f%% (allowed %.2f%%)", worst,
        tolerance);
  CHECK(skips == 0, "%ld band skips", skips);
}

int main(int argc, char **argv) {
  int seeds = argc > 1 ? atoi(argv[1]) : 200;
  if (seeds < 1)
    seeds = 1;
  wg_params def = wg_params_default();

  wg_world_generate(&a, 0xDEADBEEFu, &def);
  wg_world_generate(&b, 0xDEADBEEFu, &def);
  int same = memcmp(a.tiles, b.tiles, sizeof a.tiles) == 0 &&
             memcmp(a.height, b.height, sizeof a.height) == 0;
  printf("%-26s %s\n", "deterministic", same ? "yes" : "NO");
  CHECK(same, "same seed produced different worlds");

  // Without the mode filter, coverage should hit the targets exactly (to within
  // rounding). The filter is allowed to nudge it a little.
  wg_params raw = def;
  raw.mode_passes = 0.0f;
  run("defaults, no mode filter", raw, seeds, 0.01);
  run("defaults", def, seeds, 2.0);

  wg_params wet = def;
  wet.deep_line = 40.0f;
  wet.sea_line = 70.0f;
  wet.rock_line = 97.0f;
  run("wet world", wet, seeds, 2.0);

  wg_params thin = def;
  thin.deep_line = 36.0f;
  run("thin shallow band (1%)", thin, seeds, 100.0);

  wg_params squeezed = def;
  squeezed.deep_line = 48.0f;
  squeezed.sea_line = 49.0f;
  squeezed.rock_line = 50.0f;
  run("all middle bands at 1%", squeezed, seeds, 100.0);

  printf(failures ? "\n%d check(s) failed\n" : "\nall checks passed\n",
         failures);
  return failures ? 1 : 0;
}

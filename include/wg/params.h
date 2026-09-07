#ifndef WG_PARAMS_H
#define WG_PARAMS_H

#include "wg/tiles.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WG_PARAM_LIST                                                          \
  X(frequency, "frequency", 0.02f, 0.002f, 0.2f, 0.002f)                       \
  X(octaves, "octaves", 5.0f, 1.0f, 8.0f, 1.0f)                                \
  X(lacunarity, "lacunarity", 2.0f, 1.5f, 3.0f, 0.1f)                          \
  X(gain, "gain", 0.5f, 0.2f, 0.8f, 0.05f)                                     \
  X(level_water, "deep water <", 0.35f, 0.0f, 1.0f, 0.01f)                     \
  X(level_grass, "water <", 0.45f, 0.0f, 1.0f, 0.01f)                          \
  X(level_stone, "grass <", 0.70f, 0.0f, 1.0f, 0.01f)                          \
  X(mode_passes, "mode passes", 1.0f, 0.0f, 5.0f, 1.0f)

typedef struct {
#define X(field, label, def, min, max, step) float field;
  WG_PARAM_LIST
#undef X
} wg_params;

static inline wg_params wg_params_default(void) {
  wg_params p = {
#define X(field, label, def, min, max, step) .field = def,
      WG_PARAM_LIST
#undef X
  };
  return p;
}

static inline void wg_params_levels(const wg_params *p,
                                    float out[WG_TILE_COUNT - 1]) {
  _Static_assert(WG_TILE_COUNT == 4, "add a level_* knob per new tile");
  out[0] = p->level_water;
  out[1] = p->level_grass;
  out[2] = p->level_stone;
}

#ifdef __cplusplus
}
#endif

#endif

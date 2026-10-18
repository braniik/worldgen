#ifndef WG_PARAMS_H
#define WG_PARAMS_H

#include "wg/tiles.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WG_PARAM_LIST                                                          \
  X(frequency, "frequency", 0.02f, 0.002f, 0.2f, 0.002f, 0)                    \
  X(octaves, "octaves", 5.0f, 1.0f, 8.0f, 1.0f, 0)                             \
  X(lacunarity, "lacunarity", 2.0f, 1.5f, 3.0f, 0.1f, 0)                       \
  X(gain, "gain", 0.5f, 0.2f, 0.8f, 0.05f, 0)                                  \
  X(deep_line, "deep water %", 15.0f, 1.0f, 99.0f, 1.0f, 1)                    \
  X(sea_line, "sea level %", 37.0f, 1.0f, 99.0f, 1.0f, 1)                      \
  X(rock_line, "rock line %", 93.0f, 1.0f, 99.0f, 1.0f, 1)                     \
  X(mode_passes, "mode passes", 1.0f, 0.0f, 5.0f, 1.0f, 0)

typedef struct {
#define X(field, label, def, min, max, step, ordered) float field;
  WG_PARAM_LIST
#undef X
} wg_params;

static inline wg_params wg_params_default(void) {
  wg_params p = {
#define X(field, label, def, min, max, step, ordered) .field = def,
      WG_PARAM_LIST
#undef X
  };
  return p;
}

static inline void wg_params_lines(const wg_params *p,
                                   float out[WG_TILE_COUNT - 1]) {
  _Static_assert(WG_TILE_COUNT == 4, "add a *_line knob per new tile");
  out[0] = p->deep_line / 100.0f;
  out[1] = p->sea_line / 100.0f;
  out[2] = p->rock_line / 100.0f;
}

#ifdef __cplusplus
}
#endif

#endif

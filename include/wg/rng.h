#ifndef WG_RNG_H
#define WG_RNG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Constants are the "lowbias32" set from Chris Wellons' hash prospector (public
// domain).

static inline uint32_t wg_mix32(uint32_t h) {
  h ^= h >> 16;
  h *= 0x7feb352du;
  h ^= h >> 15;
  h *= 0x846ca68bu;
  h ^= h >> 16;
  return h;
}

uint32_t wg_hash2(uint32_t seed, int32_t x, int32_t y);

float wg_hash2f(uint32_t seed, int32_t x, int32_t y);

uint32_t wg_seed_next(uint32_t seed);

#ifdef __cplusplus
}
#endif

#endif

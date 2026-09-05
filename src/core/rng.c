#include "wg/rng.h"

uint32_t wg_hash2(uint32_t seed, int32_t x, int32_t y) {
  uint32_t h = wg_mix32(seed);
  h = wg_mix32(h ^ (uint32_t)x);
  h = wg_mix32(h ^ (uint32_t)y);
  return h;
}

float wg_hash2f(uint32_t seed, int32_t x, int32_t y) {
  // Top 24 bits into the float mantissa.
  return (float)(wg_hash2(seed, x, y) >> 8) * (1.0f / 16777216.0f);
}

uint32_t wg_seed_next(uint32_t seed) {
  // Weyl step (golden ratio) then mix
  return wg_mix32(seed + 0x9E3779B9u);
}

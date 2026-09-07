#include "wg/noise.h"

#include <math.h>

#include "wg/rng.h"

static inline float smootherstep(float t) {
  return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

float wg_value_noise(uint32_t seed, float x, float y) {
  float fx = floorf(x), fy = floorf(y);
  int32_t ix = (int32_t)fx, iy = (int32_t)fy;
  float tx = smootherstep(x - fx), ty = smootherstep(y - fy);

  float v00 = wg_hash2f(seed, ix, iy);
  float v10 = wg_hash2f(seed, ix + 1, iy);
  float v01 = wg_hash2f(seed, ix, iy + 1);
  float v11 = wg_hash2f(seed, ix + 1, iy + 1);

  return lerp(lerp(v00, v10, tx), lerp(v01, v11, tx), ty);
}

float wg_fbm(uint32_t seed, float x, float y, int octaves, float lacunarity,
             float gain) {
  float sum = 0.0f, amp = 1.0f, norm = 0.0f;
  for (int i = 0; i < octaves; i++) {
    sum += amp * wg_value_noise(seed + (uint32_t)i * 0x9E3779B9u, x, y);
    norm += amp;
    x *= lacunarity;
    y *= lacunarity;
    amp *= gain;
  }
  return norm > 0.0f ? sum / norm : 0.0f;
}

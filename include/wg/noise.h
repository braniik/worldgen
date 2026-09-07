#ifndef WG_NOISE_H
#define WG_NOISE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

float wg_value_noise(uint32_t seed, float x, float y);

float wg_fbm(uint32_t seed, float x, float y, int octaves, float lacunarity,
             float gain);

#ifdef __cplusplus
}
#endif

#endif

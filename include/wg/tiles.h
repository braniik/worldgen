#ifndef WG_TILES_H
#define WG_TILES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WG_TILE_LIST                                                           \
  X(DEEP_WATER, 0x1b3a5cu)                                                     \
  X(WATER, 0x2d5a8cu)                                                          \
  X(GRASS, 0x4a7c3fu)                                                          \
  X(STONE, 0x7a7a7au)

typedef enum {
#define X(name, color) WG_TILE_##name,
  WG_TILE_LIST
#undef X
      WG_TILE_COUNT
} wg_tile;

static const uint32_t wg_tile_color[WG_TILE_COUNT] = {
#define X(name, color) [WG_TILE_##name] = color,
    WG_TILE_LIST
#undef X
};

static const char *const wg_tile_name[WG_TILE_COUNT] = {
#define X(name, color) [WG_TILE_##name] = #name,
    WG_TILE_LIST
#undef X
};

#ifdef __cplusplus
}
#endif

#endif

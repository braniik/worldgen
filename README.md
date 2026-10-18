# WORLDGEN

A from-scratch tile world generator in C.

## The idea

I wanted to understand how procedural worlds actually get made, so I'm building one from scratch

Every world grows from a single 32-bit seed. The same seed gives the same world. You tweak the knobs live and the map regenerates as you go.

## Deadbeef Seed


## Features

- **Seeded worlds:** Pass a seed on the command line to reproduce any world, or press <kbd>g</kbd> to derive the next one.
- **Stateless generation:** Every tile's height is computed from `(seed, x, y)` alone, in any order, with no hidden state to get out of sync.
- **Bit-identical everywhere:** The same seed gives the same heights on gcc or clang, with or without `-march=native`. Fused multiply-add is turned off for every build.
- **fBm terrain:** Layered value noise with frequency, octaves, lacunarity and gain, all tunable live.
- **Coverage-based bands:** Set how much of the map is deep water, sea and rock in percent, and every seed lands within a fraction of a percent of it.
- **Clean coastlines:** Bands never skip. Deep water never touches grass, and stone never touches water. A rim pass guarantees it even for very thin bands.
- **Live parameter menu:** vim keys to select and adjust, and the world regenerates on every change. Band levels stay in order on their own.
- **Mode filter:** Smoothing passes that absorb stray single tiles.
- **Tile borders:** Toggle the grid. It switches off on its own when tiles get too small to afford it.
- **Headless checks:** `make check` tests determinism, coverage and coastlines across hundreds of seeds.
- **Keyboard only.** Nothing needs a mouse.

## Keys

| Key | What it does |
|-----|--------------|
| <kbd>g</kbd> | Generate the next world from the current seed |
| <kbd>m</kbd> | Toggle the parameter menu |
| <kbd>j</kbd> / <kbd>k</kbd> | Select a parameter (menu open) |
| <kbd>h</kbd> / <kbd>l</kbd> | Adjust by one step (menu open) |
| <kbd>H</kbd> / <kbd>L</kbd> | Adjust by ten steps (menu open) |
| <kbd>b</kbd> | Toggle tile borders |
| <kbd>F11</kbd> | Toggle fullscreen |
| <kbd>Esc</kbd> | Quit |

## Parameters

| Parameter | What it does |
|-----------|--------------|
| frequency | Feature size, lower means bigger continents |
| octaves | Layers of detail stacked on top of each other |
| lacunarity | How much finer each layer gets |
| gain | How much weaker each layer gets |
| deep water % | Share of the map that's deep water |
| sea level % | Share of the map under water, deep and shallow together |
| rock line % | Share of the map below the rock line, everything above is stone |
| mode passes | Smoothing passes over the finished tiles |

## Building

### Arch

```bash
sudo pacman -S sdl3 base-devel
```

### Build from source

1. Clone the repo
```bash
git clone https://github.com/braniik/worldgen.git
```

2. Build and run
```bash
make run # random seed
make run ARGS=0xDEADBEEF # a specific seed
```

Other targets:
```bash
make debug # ASan/UBSan build in build/debug
make MODE=debug run # run the debug build
make check # headless checks
make MODE=debug check # the same checks under ASan/UBSan
```

The seed is printed to stdout and shown in the window title.

## Stack

- C11, plain GNU Make, gcc or clang
- SDL3 for the window, input and presenting frames, and nothing else
- Software rendering into a `uint32_t` framebuffer
- `lowbias32` hash from Chris Wellons' [hash prospector](https://github.com/skeeto/hash-prospector) (public domain)
- `font8x8` bitmap font by Daniel Hepper (public domain)
- No other dependencies

## Project structure

```
├── Makefile                  — release/debug builds in separate dirs, `make check`, -ffp-contract=off
├── include/wg/
│   ├── framebuffer.h         — plain pixel buffer, filled rects, 8x8 text
│   ├── menu.h                — parameter menu state, key results (ignored/moved/changed)
│   ├── noise.h               — value noise and fBm
│   ├── params.h              — every tunable knob, defined once as an X-macro list
│   ├── platform.h            — the only interface to the OS: events, size, present, title
│   ├── render.h              — world to framebuffer
│   ├── rng.h                 — lowbias32 mixer, stateless 2D hash, seed chaining
│   ├── tiles.h               — tile names and colours as an X-macro list
│   └── world.h               — the world: height field, tiles, resolved band levels
├── src/
│   ├── main.c                — event loop, keys, regenerate and redraw on change
│   ├── core/
│   │   ├── font8x8_basic.h   — public domain 8x8 bitmap font
│   │   ├── framebuffer.c     — resize, clear, rects, text
│   │   ├── menu.c            — menu drawing, adjusting, keeping band levels in order
│   │   ├── noise.c           — value noise with smootherstep, fBm octaves
│   │   ├── render.c          — integer-scaled tiles, optional borders
│   │   ├── rng.c             — wg_hash2, float hash, next seed
│   │   └── world.c           — height field, coverage to levels, banding, mode filter, rim pass
│   └── platform/
│       └── sdl3.c            — the one file that touches SDL
└── tools/
    └── check.c               — headless checks: determinism, coverage, no skipped bands
```

## Roadmap

- [x] 0.1 — Stateless hash generation, SDL3 render pipeline
- [x] 0.2 — fBm terrain, parameter menu
- [x] 0.3 — Stored height field, coverage-based bands, rim pass, `make check`, reproducible builds
- [ ] 0.4 — Better terrain (gradient noise, domain warping, continent shapes, hillshading)
- [ ] 0.5 — Climate and biomes (moisture, temperature, biome lookup table, beaches, snow caps)
- [ ] 0.6 — Feature layer (trees, bushes, boulders on a jittered grid, species per biome)
- [ ] 0.7 — Water (lakes from depression filling, rivers from flow accumulation)
- [ ] 0.8 — Camera and keyboard editing (tile cursor, zoom, vim-style modes, `:` commands, undo)
- [ ] 0.9 — A world that changes over time (deterministic tick, growth, spreading fire, seasons)
- [ ] 1.0 — Saving and loading (seed, params and an edit log, replayed on load)

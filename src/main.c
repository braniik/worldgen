#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "wg/framebuffer.h"
#include "wg/menu.h"
#include "wg/platform.h"
#include "wg/render.h"
#include "wg/rng.h"
#include "wg/world.h"

static wg_world world;
static wg_params params;
static wg_menu menu;
static bool grid = true;

static uint32_t initial_seed(int argc, char **argv) {
  /* worldgen 0xDEADBEEF or worldgen 12345 reproduces a run exactly. */
  if (argc > 1)
    return (uint32_t)strtoul(argv[1], NULL, 0);
  return wg_mix32((uint32_t)time(NULL) ^ ((uint32_t)clock() << 16));
}

static void regenerate(wg_platform *p, uint32_t seed) {
  wg_world_generate(&world, seed, &params);

  char title[64];
  snprintf(title, sizeof title, "worldgen | seed 0x%08x", seed);
  wg_platform_set_title(p, title);
  printf("seed 0x%08x\n", seed);
}

static void redraw(wg_platform *p, wg_framebuffer *fb) {
  wg_render_world(&world, fb, grid);
  wg_menu_render(&menu, &params, fb);
  wg_platform_present(p, fb);
}

int main(int argc, char **argv) {
  setvbuf(stdout, NULL, _IOLBF, 0);

  uint32_t seed = initial_seed(argc, argv);
  params = wg_params_default();

  wg_platform *p = wg_platform_create("worldgen", 800, 800);
  if (!p)
    return 1;

  int w, h;
  wg_platform_get_size(p, &w, &h);

  wg_framebuffer fb;
  if (!wg_fb_init(&fb, w, h)) {
    wg_platform_destroy(p);
    return 1;
  }

  regenerate(p, seed);
  redraw(p, &fb);

  bool running = true;
  bool dirty = false;
  wg_event ev;

  while (running && wg_platform_wait_event(p, &ev)) {
    switch (ev.type) {
    case WG_EVENT_QUIT:
      running = false;
      break;

    case WG_EVENT_KEY_DOWN:
      if (menu.open) {
        wg_menu_result r = wg_menu_key(&menu, &params, ev.key);
        if (r == WG_MENU_CHANGED)
          regenerate(p, seed);
        if (r != WG_MENU_IGNORED) {
          dirty = true;
          break;
        }
      }
      if (ev.repeat)
        break;
      switch (ev.key) {
      case WG_KEY_ESCAPE:
        running = false;
        break;
      case 'g':
        seed = wg_seed_next(seed);
        regenerate(p, seed);
        dirty = true;
        break;
      case 'm':
        menu.open = !menu.open;
        dirty = true;
        break;
      case 'b':
        grid = !grid;
        dirty = true;
        break;
      case WG_KEY_F11:
        wg_platform_toggle_fullscreen(p);
        break;
      default:
        break;
      }
      break;

    case WG_EVENT_RESIZE:
      wg_platform_get_size(p, &w, &h);
      wg_fb_resize(&fb, w, h);
      dirty = true;
      break;

    case WG_EVENT_REDRAW:
      dirty = true;
      break;

    default:
      break;
    }

    if (dirty && running) {
      redraw(p, &fb);
      dirty = false;
    }
  }

  wg_fb_free(&fb);
  wg_platform_destroy(p);
  return 0;
}

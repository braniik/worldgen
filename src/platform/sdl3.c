#include <SDL3/SDL.h>
#include <stdlib.h>

#include "wg/platform.h"

struct wg_platform {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture; // streaming, recreated when the fb size changes
  int tex_w, tex_h;
};

wg_platform *wg_platform_create(const char *title, int w, int h) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init: %s", SDL_GetError());
    return NULL;
  }

  wg_platform *p = calloc(1, sizeof *p);
  if (!p) {
    SDL_Quit();
    return NULL;
  }

  p->window = SDL_CreateWindow(
      title, w, h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if (!p->window) {
    SDL_Log("SDL_CreateWindow: %s", SDL_GetError());
    wg_platform_destroy(p);
    return NULL;
  }

  p->renderer = SDL_CreateRenderer(p->window, NULL);
  if (!p->renderer) {
    SDL_Log("SDL_CreateRenderer: %s", SDL_GetError());
    wg_platform_destroy(p);
    return NULL;
  }

  return p;
}

void wg_platform_destroy(wg_platform *p) {
  if (!p)
    return;
  if (p->texture)
    SDL_DestroyTexture(p->texture);
  if (p->renderer)
    SDL_DestroyRenderer(p->renderer);
  if (p->window)
    SDL_DestroyWindow(p->window);
  free(p);
  SDL_Quit();
}

static int map_key(SDL_Keycode k) {
  if (k >= SDLK_F1 && k <= SDLK_F12)
    return WG_KEY_F1 + (int)(k - SDLK_F1);
  if (k < 128)
    return (int)k; // ASCII ESC is 27
  return 0;
}

bool wg_platform_wait_event(wg_platform *p, wg_event *ev) {
  (void)p;

  SDL_Event e;
  if (!SDL_WaitEvent(&e)) {
    SDL_Log("SDL_WaitEvent: %s", SDL_GetError());
    return false;
  }

  ev->type = WG_EVENT_NONE;
  ev->key = 0;
  ev->repeat = false;

  switch (e.type) {
  case SDL_EVENT_QUIT:
  case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
    ev->type = WG_EVENT_QUIT;
    break;

  case SDL_EVENT_KEY_DOWN:
    ev->type = WG_EVENT_KEY_DOWN;
    ev->key = map_key(e.key.key);
    ev->repeat = e.key.repeat;
    if (ev->key >= 'a' && ev->key <= 'z' && (e.key.mod & SDL_KMOD_SHIFT))
      ev->key -= 'a' - 'A';
    break;

  case SDL_EVENT_WINDOW_RESIZED:
  case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
    ev->type = WG_EVENT_RESIZE;
    break;

  case SDL_EVENT_WINDOW_EXPOSED:
    ev->type = WG_EVENT_REDRAW;
    break;

  default:
    break;
  }

  return true;
}

void wg_platform_get_size(wg_platform *p, int *w, int *h) {
  if (!SDL_GetWindowSizeInPixels(p->window, w, h)) {
    SDL_Log("SDL_GetWindowSizeInPixels: %s", SDL_GetError());
    *w = *h = 1;
  }
}

bool wg_platform_present(wg_platform *p, const wg_framebuffer *fb) {
  if (!p->texture || p->tex_w != fb->w || p->tex_h != fb->h) {
    if (p->texture)
      SDL_DestroyTexture(p->texture);
    p->texture = SDL_CreateTexture(p->renderer, SDL_PIXELFORMAT_XRGB8888,
                                   SDL_TEXTUREACCESS_STREAMING, fb->w, fb->h);
    if (!p->texture) {
      SDL_Log("SDL_CreateTexture: %s", SDL_GetError());
      return false;
    }
    SDL_SetTextureScaleMode(p->texture, SDL_SCALEMODE_NEAREST);
    p->tex_w = fb->w;
    p->tex_h = fb->h;
  }

  SDL_UpdateTexture(p->texture, NULL, fb->px, fb->w * (int)sizeof(uint32_t));

  SDL_SetRenderDrawColor(p->renderer, 0, 0, 0, 255);
  SDL_RenderClear(p->renderer);
  SDL_RenderTexture(p->renderer, p->texture, NULL, NULL);
  SDL_RenderPresent(p->renderer);
  return true;
}

void wg_platform_toggle_fullscreen(wg_platform *p) {
  bool fs = (SDL_GetWindowFlags(p->window) & SDL_WINDOW_FULLSCREEN) != 0;
  SDL_SetWindowFullscreen(p->window, !fs);
}

void wg_platform_set_title(wg_platform *p, const char *title) {
  SDL_SetWindowTitle(p->window, title);
}

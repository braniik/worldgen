#include "wg/menu.h"

#include <stdio.h>

typedef struct {
  const char *label;
  float min, max, step;
} param_desc;

static const param_desc descs[] = {
#define X(field, label, def, min, max, step) {label, min, max, step},
    WG_PARAM_LIST
#undef X
};
enum { N_PARAMS = sizeof descs / sizeof descs[0] };

static float *field_ptr(wg_params *p, int i) {
  float *fields[] = {
#define X(field, label, def, min, max, step) &p->field,
      WG_PARAM_LIST
#undef X
  };
  return fields[i];
}

static float clampf(float v, float lo, float hi) {
  return v < lo ? lo : v > hi ? hi : v;
}

bool wg_menu_key(wg_menu *m, wg_params *p, int key) {
  int dir = 0;
  switch (key) {
  case 'j':
    m->cursor = (m->cursor + 1) % N_PARAMS;
    return false;
  case 'k':
    m->cursor = (m->cursor + N_PARAMS - 1) % N_PARAMS;
    return false;
  case 'h':
    dir = -1;
    break;
  case 'l':
    dir = 1;
    break;
  case 'H':
    dir = -10;
    break;
  case 'L':
    dir = 10;
    break;
  default:
    return false;
  }
  const param_desc *d = &descs[m->cursor];
  float *v = field_ptr(p, m->cursor);
  float nv = clampf(*v + dir * d->step, d->min, d->max);
  if (nv == *v)
    return false;
  *v = nv;
  return true;
}

void wg_menu_render(const wg_menu *m, const wg_params *p, wg_framebuffer *fb) {
  if (!m->open)
    return;

  const int scale = 2, pad = 8, line = WG_FONT_H * scale + 4;
  const int cols = 24;
  int w = cols * WG_FONT_W * scale + 2 * pad;
  int h = (N_PARAMS + 1) * line + 2 * pad;

  wg_fb_fill_rect(fb, 0, 0, w, h, 0x101010u);

  char buf[32];
  int y = pad;
  wg_fb_text(fb, pad, y, scale, "j/k select  h/l adjust", 0x808080u);
  y += line;

  for (int i = 0; i < N_PARAMS; i++, y += line) {
    float v = *field_ptr((wg_params *)p, i);
    if (descs[i].step >= 1.0f)
      snprintf(buf, sizeof buf, "%c %-13s %6d", i == m->cursor ? '>' : ' ',
               descs[i].label, (int)v);
    else
      snprintf(buf, sizeof buf, "%c %-13s %6.3f", i == m->cursor ? '>' : ' ',
               descs[i].label, (double)v);
    wg_fb_text(fb, pad, y, scale, buf, i == m->cursor ? 0xffffffu : 0xb0b0b0u);
  }
}

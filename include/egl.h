#ifndef GF_EGL_H
#define GF_EGL_H

#include <EGL/egl.h>
#include <stdbool.h>
#include <wayland-client-core.h>
#include <wayland-egl-core.h>

struct gf_egl_state {
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
};

bool init_egl(struct gf_egl_state *gf_egl_state, struct wl_display *wl_display,
              struct wl_egl_window *wl_egl_window);

#endif

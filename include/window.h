#ifndef GF_WINDOW_H
#define GF_WINDOW_H

#include <stdbool.h>

#include "cursor-shape.h"

struct gf_window {
  struct wl_display *display;
  struct wl_compositor *compositor;
  struct xdg_wm_base *xdg_wm_base;
  struct wl_surface *surface;
  struct xdg_surface *xdg_surface;
  struct xdg_toplevel *xdg_toplevel;
  struct wl_egl_window *wl_egl_window;
  struct wl_seat *wl_seat;
  struct wl_pointer *wl_pointer;
  struct wp_cursor_shape_manager_v1 *cursor_shape_manager;
  struct wp_cursor_shape_device_v1 *cursor_shape_device;
};

bool init_gf_window(struct gf_window *window);

#endif

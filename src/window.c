#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl-core.h>

#include "xdg-shell.h"

#include "window.h"


/* wl_registry Listener */
static void global_registry_handler(void *data, struct wl_registry *registry,
                                    uint32_t id, const char *interface,
                                    uint32_t version) {
  struct gf_window *window = data;

  if (strcmp(interface, "wl_compositor") == 0) {
    window->compositor =
        wl_registry_bind(registry, id, &wl_compositor_interface, 6);
  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    window->xdg_wm_base =
        wl_registry_bind(registry, id, &xdg_wm_base_interface, 5);
  }
}
static void global_registry_remover(void *data, struct wl_registry *registry,
                                    uint32_t id) {
  printf("Got a registry losing event for %d\n", id);
}
static const struct wl_registry_listener registry_listener = {
    global_registry_handler, global_registry_remover};


/* xdg_wm_base Listener */
static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base,
                             uint32_t serial) {
  xdg_wm_base_pong(xdg_wm_base, serial);
}
static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping};


/* xdg_surface Listener */
static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
                                  uint32_t serial) {
  xdg_surface_ack_configure(xdg_surface, serial);
}
static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure};


/* xdg_toplevel Listener */
int32_t old_w, old_h;
static void xdg_toplevel_configure(void *data,
                                   struct xdg_toplevel *xdg_toplevel, int32_t w,
                                   int32_t h, struct wl_array *states) {
  struct gf_window *window = data;
  // no window geometry event, ignore
  if (w == 0 && h == 0)
    return;

  // window resized
  if (old_w != w && old_h != h) {
    old_w = w;
    old_h = h;

    wl_egl_window_resize(window->wl_egl_window, w, h, 0, 0);
    wl_surface_commit(window->surface);
  }
}
static void xdg_toplevel_wm_capabilities(void *data,
                                         struct xdg_toplevel *xdg_toplevel,
                                         struct wl_array *capabilities) {
  printf("xdg-toplevel wm_capabilities:\n");

  int *capability;
  wl_array_for_each(capability, capabilities) {
    printf("    Capability - '");

#define CASE_STR(value)                                                        \
  case value: printf(#value); break;

    switch (*capability) {
      CASE_STR(XDG_TOPLEVEL_WM_CAPABILITIES_WINDOW_MENU)
      CASE_STR(XDG_TOPLEVEL_WM_CAPABILITIES_MAXIMIZE)
      CASE_STR(XDG_TOPLEVEL_WM_CAPABILITIES_FULLSCREEN)
      CASE_STR(XDG_TOPLEVEL_WM_CAPABILITIES_MINIMIZE)
    }

#undef CASE_STR

    printf("'.\n");
  }
}
static const struct xdg_toplevel_listener toplevel_listener = {
    .configure       = xdg_toplevel_configure,
    .wm_capabilities = xdg_toplevel_wm_capabilities};


bool init_gf_window(struct gf_window *window) {
  int err;

  /* connect to compositor */
  window->display = wl_display_connect(NULL);
  assert(window->display != NULL);

  struct wl_registry *registry = wl_display_get_registry(window->display);
  wl_registry_add_listener(registry, &registry_listener, window);

  wl_display_dispatch(window->display);
  wl_display_roundtrip(window->display);
  assert(window->xdg_wm_base != NULL);
  assert(window->compositor != NULL);

  xdg_wm_base_add_listener(window->xdg_wm_base, &xdg_wm_base_listener, NULL);

  /* create wl_surface */
  window->surface = wl_compositor_create_surface(window->compositor);
  assert(window->surface != NULL);

  /* create xdg_surface */
  window->xdg_surface =
      xdg_wm_base_get_xdg_surface(window->xdg_wm_base, window->surface);
  assert(window->xdg_surface != NULL);

  err = xdg_surface_add_listener(window->xdg_surface, &xdg_surface_listener,
                                 NULL);
  assert(err == 0);

  /* create toplevel */
  window->xdg_toplevel = xdg_surface_get_toplevel(window->xdg_surface);
  assert(window->xdg_toplevel);
  xdg_toplevel_set_title(window->xdg_toplevel, "test title");
  err = xdg_toplevel_add_listener(window->xdg_toplevel, &toplevel_listener,
                                  window);
  assert(err == 0);
  wl_surface_commit(window->surface);

  /* create wl window */
  window->wl_egl_window = wl_egl_window_create(window->surface, 480, 360);
  assert(window->wl_egl_window != NULL);
  return window;
}

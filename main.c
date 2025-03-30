#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GL/gl.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl-core.h>

#include "xdg_shell.h"

struct wl_display *display = NULL;
struct wl_compositor *compositor = NULL;
struct xdg_wm_base *xdg_wm_base = NULL;
struct wl_surface *surface = NULL;
struct xdg_surface *xdg_surface = NULL;
struct xdg_toplevel *xdg_toplevel = NULL;

// clang-format off
EGLint const attrib_list[] = {
        EGL_RED_SIZE, 4,
        EGL_GREEN_SIZE, 4,
        EGL_BLUE_SIZE, 4,
        EGL_NONE
};
// clang-format on

static void global_registry_handler(void *data, struct wl_registry *registry,
                                    uint32_t id, const char *interface,
                                    uint32_t version) {
  printf("Got a registry event for %s id %d\n", interface, id);
  if (strcmp(interface, "wl_compositor") == 0) {
    compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 6);
  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    xdg_wm_base = wl_registry_bind(registry, id, &xdg_wm_base_interface, 5);
  }
}

static void global_registry_remover(void *data, struct wl_registry *registry,
                                    uint32_t id) {
  printf("Got a registry losing event for %d\n", id);
}

static const struct wl_registry_listener registry_listener = {
    global_registry_handler, global_registry_remover};

// TODO: Create listener.
//static const struct xdg_toplevel_listener toplevel_listener = {}

int main() {
  EGLDisplay egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  assert(egl_display != EGL_NO_DISPLAY);
  EGLBoolean success;
  EGLint major;
  EGLint minor;
  success = eglInitialize(egl_display, &major, &minor);
  assert(success);
  printf("EGL Version - v%i.%i\n", major, minor);

  /* config selection */
  EGLConfig config;
  EGLint num_config;
  success = eglChooseConfig(egl_display, attrib_list, &config, 1, &num_config);
  assert(success);
  assert(num_config == 1);

  /* connect to compositor */
  display = wl_display_connect(NULL);
  assert(display != NULL);

  struct wl_registry *registry = wl_display_get_registry(display);
  wl_registry_add_listener(registry, &registry_listener, NULL);

  wl_display_dispatch(display);
  wl_display_roundtrip(display);
  assert(xdg_wm_base != NULL);
  assert(compositor != NULL);

  /* create wl_surface */
  surface = wl_compositor_create_surface(compositor);
  assert(surface != NULL);

  /* create xdg_surface */
  xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, surface);
  assert(xdg_surface != NULL);

  /* create toplevel */
  xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
  assert(xdg_toplevel);

  /* create wl window */
  // struct wl_surface surface;
  // wl_egl_window_create(struct wl_surface *surface, int width, int height)

  /* create context */
}

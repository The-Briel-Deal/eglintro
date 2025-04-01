#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GL/gl.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <wayland-util.h>

#include "xdg_shell.h"

struct wl_display *display = NULL;
struct wl_compositor *compositor = NULL;
struct xdg_wm_base *xdg_wm_base = NULL;
struct wl_surface *surface = NULL;
struct xdg_surface *xdg_surface = NULL;
struct xdg_toplevel *xdg_toplevel = NULL;
struct wl_egl_window *egl_window = NULL;
struct wl_region *region = NULL;
EGLSurface egl_surface = EGL_NO_SURFACE;
EGLContext egl_context = EGL_NO_CONTEXT;

static const EGLint config_attribs[] = {EGL_SURFACE_TYPE,
                                        EGL_WINDOW_BIT,
                                        EGL_RENDERABLE_TYPE,
                                        EGL_OPENGL_ES2_BIT,
                                        EGL_RED_SIZE,
                                        8,
                                        EGL_GREEN_SIZE,
                                        8,
                                        EGL_BLUE_SIZE,
                                        8,
                                        EGL_NONE};

static const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2,
                                         EGL_NONE, EGL_NONE};

static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base,
                             uint32_t serial) {
  printf("ponged (:\n");
  xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping};

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

static void xdg_toplevel_configure(void *data,
                                   struct xdg_toplevel *xdg_toplevel,
                                   int32_t width, int32_t height,
                                   struct wl_array *states) {
  printf("Toplevel Configured\n");
}

static void xdg_toplevel_wm_capabilities(void *data,
                                         struct xdg_toplevel *xdg_toplevel,
                                         struct wl_array *capabilities) {
  printf("xdg toplevel wm_capabilities:\n");

  int *capability;
  wl_array_for_each(capability, capabilities) {
    printf("Capability - %i\n", *capability);
  }
}

static const struct xdg_toplevel_listener toplevel_listener = {
    .configure = xdg_toplevel_configure,
    .wm_capabilities = xdg_toplevel_wm_capabilities};

static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
                                  uint32_t serial) {
  printf("xdg_surface configured\n");
  xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure};

const char *eglGetErrorString(EGLint error) {
#define CASE_STR(value)                                                        \
  case value:                                                                  \
    return #value;
  switch (error) {
    CASE_STR(EGL_SUCCESS)
    CASE_STR(EGL_NOT_INITIALIZED)
    CASE_STR(EGL_BAD_ACCESS)
    CASE_STR(EGL_BAD_ALLOC)
    CASE_STR(EGL_BAD_ATTRIBUTE)
    CASE_STR(EGL_BAD_CONTEXT)
    CASE_STR(EGL_BAD_CONFIG)
    CASE_STR(EGL_BAD_CURRENT_SURFACE)
    CASE_STR(EGL_BAD_DISPLAY)
    CASE_STR(EGL_BAD_SURFACE)
    CASE_STR(EGL_BAD_MATCH)
    CASE_STR(EGL_BAD_PARAMETER)
    CASE_STR(EGL_BAD_NATIVE_PIXMAP)
    CASE_STR(EGL_BAD_NATIVE_WINDOW)
    CASE_STR(EGL_CONTEXT_LOST)
  default:
    return "Unknown";
  }
#undef CASE_STR
}

int main() {

  /* connect to compositor */
  display = wl_display_connect(NULL);
  assert(display != NULL);

  struct wl_registry *registry = wl_display_get_registry(display);
  wl_registry_add_listener(registry, &registry_listener, NULL);

  wl_display_dispatch(display);
  wl_display_roundtrip(display);
  assert(xdg_wm_base != NULL);
  assert(compositor != NULL);

  xdg_wm_base_add_listener(xdg_wm_base, &xdg_wm_base_listener, NULL);

  /* create wl_surface */
  surface = wl_compositor_create_surface(compositor);
  assert(surface != NULL);

  /* create xdg_surface */
  xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, surface);
  assert(xdg_surface != NULL);

  int err;
  err = xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, NULL);
  assert(err == 0);

  /* create toplevel */
  xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
  assert(xdg_toplevel);

  xdg_toplevel_set_title(xdg_toplevel, "test title");

  err = xdg_toplevel_add_listener(xdg_toplevel, &toplevel_listener, NULL);
  assert(err == 0);

  wl_surface_commit(surface);

  /* create wl window */
  region = wl_compositor_create_region(compositor);

  wl_region_add(region, 0, 0, 480, 360);
  wl_surface_set_opaque_region(surface, region);
  egl_window = wl_egl_window_create(surface, 480, 360);
  assert(egl_window != NULL);

  EGLDisplay egl_display = eglGetDisplay(display);
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
  success =
      eglChooseConfig(egl_display, config_attribs, &config, 1, &num_config);
  assert(success);
  assert(num_config == 1);

  /* create context */
  egl_context =
      eglCreateContext(egl_display, config, EGL_NO_CONTEXT, context_attribs);
  assert(egl_context != EGL_NO_CONTEXT);

  egl_surface = eglCreateWindowSurface(egl_display, config,
                                       (EGLNativeWindowType)egl_window, NULL);
  err = eglGetError();
  printf("eglError = %s\n", eglGetErrorString(err));
  assert(egl_surface != NULL);
}

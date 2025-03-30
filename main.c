#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GL/gl.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl-core.h>

struct wl_display *display = NULL;
struct wl_compositor *compositor = NULL;
struct wl_surface *surface = NULL;
struct wl_shell *shell = NULL;

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
    compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
  } else if (strcmp(interface, "wl_shell") == 0) {
    shell = wl_registry_bind(registry, id, &wl_shell_interface, 1);
  }
}

static void global_registry_remover(void *data, struct wl_registry *registry,
                                    uint32_t id) {
  printf("Got a registry losing event for %d\n", id);
}

static const struct wl_registry_listener registry_listener = {
    global_registry_handler, global_registry_remover};

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

  /* create wl window */
  // struct wl_surface surface;
  // wl_egl_window_create(struct wl_surface *surface, int width, int height)

  /* create context */
}

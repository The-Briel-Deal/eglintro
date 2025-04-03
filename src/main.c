#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <wayland-util.h>

#include "draw.h"
#include "egl.h"
#include "log.h"
#include "window.h"


int main() {
  struct gf_window window;
  init_gf_window(&window);
  struct gf_egl_state egl_state;
  init_egl(&egl_state, window.display, window.wl_egl_window);

  gf_log(INFO_LOG, "Version %s", glGetString(GL_VERSION));

  gf_compile_shaders();
  gf_create_triangle();
  while (true) {
    wl_display_dispatch_pending(window.display);

    glClearColor(1.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    gf_draw_triangle();

    eglSwapBuffers(egl_state.display, egl_state.surface);
  }
}

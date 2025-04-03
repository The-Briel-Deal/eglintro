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

const char *vert_shader_src =
    "#version 450 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "out vec4 vertexColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "    vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
    "}\n";

const char *frag_shader_src =
    "#version 450 core\n"
    "in vec4 vertexColor;\n"
    "out vec4 FragColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    FragColor = vertexColor;\n"
    "}\n";

struct triangle_verts triangle_verts = {
    {.x = 0.5,  .y = -0.5},
    {.x = 0.0,  .y = 0.5 },
    {.x = -0.5, .y = -0.5},
};

int main() {
  struct gf_window window;
  init_gf_window(&window);
  struct gf_egl_state egl_state;
  init_egl(&egl_state, window.display, window.wl_egl_window);

  gf_log(INFO_LOG, "Version %s", glGetString(GL_VERSION));

  struct shader *shader = gf_compile_shaders(vert_shader_src, frag_shader_src);
  struct triangle_obj *triangle = gf_create_triangle(&triangle_verts);
  gf_set_triangle_shader(triangle, shader);
  while (true) {
    wl_display_dispatch_pending(window.display);

    glClearColor(1.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    gf_draw_triangle(triangle);

    eglSwapBuffers(egl_state.display, egl_state.surface);
  }
}

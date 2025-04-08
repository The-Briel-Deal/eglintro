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

#include "common.h"
#include "egl.h"
#include "log.h"
#include "render.h"
#include "window.h"


const char *vert_shader_src =
    "#version 450 core\n"
    "layout (location = " TO_STR(GF_ATTRIB_VERT_LOCATION) ") in vec2 aPos;\n"
		"layout (location = " TO_STR(GF_UNIFORM_TRANSFORM_MAT_LOCATION) ") uniform mat4 model;"
		"layout (location = " TO_STR(GF_UNIFORM_PROJECTION_MAT_LOCATION) ") uniform mat4 projection;"
		"\n"
    "out vec4 vertexColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);\n"
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

struct box_verts square_verts = {
    {.x = 50, .y = 50},
    {.x = -50, .y = 50},
    {.x = 50, .y = -50},
    {.x = -50, .y = -50},
};

int main() {
  struct gf_window window;
  init_gf_window(&window);
  struct gf_egl_state egl_state;
  init_egl(&egl_state, window.display, window.wl_egl_window);

  gf_log(INFO_LOG, "Version %s", glGetString(GL_VERSION));

  struct shader *shader = gf_compile_shaders(vert_shader_src, frag_shader_src);
  struct gf_obj *square = gf_obj_create_box(&square_verts);
  gf_obj_set_shader(square, shader);
  while (true) {
    wl_display_dispatch_pending(window.display);

    tf_scale scale = gf_obj_get_scale(square);

    gf_obj_set_scale(square,
                     (tf_scale){.x = scale.x + 0.001f, .y = scale.y + 0.001f});

    gf_obj_rotate_by(square, 0.01f);

    gf_obj_commit_state(square);

    glClearColor(1.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    gf_obj_draw(square);

    eglSwapBuffers(egl_state.display, egl_state.surface);
  }
}

#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <wayland-util.h>

#include "common.h"
#include "draw.h"
#include "gf_math.h"
#include "log.h"

#define TRIANGLE_OBJ_LIST_MAX   128
#define BOX_OBJ_LIST_MAX        128
#define SHADER_PROGRAM_LIST_MAX 128

static const GLuint BOX_INDEX_ORDER[] = {0, 1, 3, 0, 2, 3};

struct render_state {
  struct viewport_dimensions {
    int height;
    int width;
    bool dirty;
  } viewport;
  struct transform {
    float scale_x;
    float scale_y;
    bool dirty;
  } transform;
};

struct render_state render_state = {
    .viewport =
        {
                   .height = GF_DEFAULT_WINDOW_HEIGHT,
                   .width  = GF_DEFAULT_WINDOW_WIDTH,
                   .dirty  = true,
                   },
    .transform = {
                   .scale_x = 3,
                   .scale_y = 3,
                   .dirty   = true,
                   }
};

struct shader {
  GLuint vert;
  GLuint frag;
  GLuint program;
};

struct shader_program_list {
  int count;
  int capacity;
  struct shader shaders[SHADER_PROGRAM_LIST_MAX];
};

static struct shader_program_list shader_program_list = {
    .count    = 0,
    .capacity = SHADER_PROGRAM_LIST_MAX,
};

struct triangle_obj {
  GLuint vbo;
  GLuint vao;
  struct shader *shader;
};

struct triangle_obj_list {
  int count;
  int capacity;
  struct triangle_obj objs[TRIANGLE_OBJ_LIST_MAX];
};

static struct triangle_obj_list triangle_obj_list = {
    .count    = 0,
    .capacity = TRIANGLE_OBJ_LIST_MAX,
};

struct box_obj {
  GLuint vbo;
  GLuint vao;
  struct shader *shader;
  GLuint ebo;
};

struct box_obj_list {
  int count;
  int capacity;
  struct box_obj objs[BOX_OBJ_LIST_MAX];
};

static struct box_obj_list box_obj_list = {
    .count    = 0,
    .capacity = BOX_OBJ_LIST_MAX,
};

bool gf_draw_update_window_size(int32_t width, int32_t height) {
  if (height == render_state.viewport.height &&
      width == render_state.viewport.width) {
    return false;
  }
  render_state.viewport = (struct viewport_dimensions){
      .width = width, .height = height, .dirty = true};
  return true;
}

void gf_commit_render_state() {
  gf_log(INFO_LOG, "Commiting render state.");
  if (render_state.viewport.dirty == true) {
    render_state.viewport.dirty = false;
    mat4 perspective_matrix;
    gf_ortho(0.0f, render_state.viewport.width, 0, render_state.viewport.height,
             -1.0, 1.0, perspective_matrix);
    for (int i = 0; i < shader_program_list.count; i++) {
      struct shader *shader = &shader_program_list.shaders[i];
      gf_log(INFO_LOG, "Commiting render state to shader %i.", i);
      glProgramUniformMatrix4fv(shader->program,
                                GF_UNIFORM_PROJECTION_MAT_LOCATION, 1, false,
                                (GLfloat *)perspective_matrix);
    }
  }
}

struct shader *gf_compile_shaders(const char *vert_shader_src,
                                  const char *frag_shader_src) {
  if (shader_program_list.count + 1 >= shader_program_list.capacity) {
    gf_log(DEBUG_LOG,
           "`shader_program_list` has a count of '%i', which is greater than "
           "it's capacity of '%i'.",
           shader_program_list.count, shader_program_list.capacity);
    return NULL;
  }

  struct shader *shader =
      &shader_program_list.shaders[shader_program_list.count++];

  shader->vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(shader->vert, 1, &vert_shader_src, NULL);
  glCompileShader(shader->vert);

  shader->frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shader->frag, 1, &frag_shader_src, NULL);
  glCompileShader(shader->frag);

  shader->program = glCreateProgram();
  glAttachShader(shader->program, shader->vert);
  glAttachShader(shader->program, shader->frag);
  glLinkProgram(shader->program);

  return shader;
}

struct triangle_obj *
gf_create_triangle(const struct triangle_verts *triangle_verts) {
  if (triangle_obj_list.count + 1 >= triangle_obj_list.capacity) {
    gf_log(DEBUG_LOG,
           "`triangle_obj_list` has a count of '%i', which is greater than "
           "it's capacity of '%i'.",
           triangle_obj_list.count, triangle_obj_list.capacity);
    return NULL;
  }

  struct triangle_obj *triangle_obj =
      &triangle_obj_list.objs[triangle_obj_list.count++];
  glCreateBuffers(1, &triangle_obj->vbo);
  glNamedBufferStorage(triangle_obj->vbo, sizeof(struct triangle_verts),
                       triangle_verts, GL_DYNAMIC_STORAGE_BIT);

  glCreateVertexArrays(1, &triangle_obj->vao);
  glVertexArrayVertexBuffer(triangle_obj->vao, 0, triangle_obj->vbo, 0,
                            sizeof(struct vertex));
  glEnableVertexArrayAttrib(triangle_obj->vao, 0);
  glVertexArrayAttribFormat(triangle_obj->vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(triangle_obj->vao, 0, 0);
  return triangle_obj;
}

bool gf_set_triangle_shader(struct triangle_obj *triangle,
                            struct shader *shader) {
  if (triangle->shader == shader) {
    return false;
  }
  triangle->shader = shader;
  return true;
}

bool gf_draw_triangle(struct triangle_obj *triangle) {
  glUseProgram(triangle->shader->program);
  glBindVertexArray(triangle->vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  return true;
}

struct box_obj *gf_create_box(const struct box_verts *box_verts) {
  if (box_obj_list.count + 1 >= box_obj_list.capacity) {
    gf_log(DEBUG_LOG,
           "`box_obj_list` has a count of '%i', which is greater than "
           "it's capacity of '%i'.",
           triangle_obj_list.count, triangle_obj_list.capacity);
    return NULL;
  }

  struct box_obj *box_obj = &box_obj_list.objs[box_obj_list.count++];

  glCreateBuffers(1, &box_obj->vbo);
  glNamedBufferStorage(box_obj->vbo, sizeof(struct box_verts), box_verts,
                       GL_DYNAMIC_STORAGE_BIT);

  glCreateBuffers(1, &box_obj->ebo);
  glNamedBufferStorage(box_obj->ebo, sizeof(BOX_INDEX_ORDER), BOX_INDEX_ORDER,
                       GL_DYNAMIC_STORAGE_BIT);

  glCreateVertexArrays(1, &box_obj->vao);
  glVertexArrayVertexBuffer(box_obj->vao, 0, box_obj->vbo, 0,
                            sizeof(struct vertex));
  glVertexArrayElementBuffer(box_obj->vao, box_obj->ebo);
  glEnableVertexArrayAttrib(box_obj->vao, 0);
  glVertexArrayAttribFormat(box_obj->vao, 0, 2, GL_FLOAT, false, 0);
  glVertexArrayAttribBinding(box_obj->vao, 0, 0);
  return box_obj;
}

bool gf_set_box_shader(struct box_obj *box, struct shader *shader) {
  if (box->shader == shader) {
    return false;
  }
  box->shader = shader;
  return true;
}

bool gf_draw_box(struct box_obj *box) {
  glUseProgram(box->shader->program);
  glBindVertexArray(box->vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  return true;
}

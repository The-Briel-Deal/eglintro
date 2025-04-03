#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>
#include <stdbool.h>
#include <wayland-util.h>

#include "draw.h"
#include "log.h"

#define TRIANGLE_OBJ_LIST_MAX   128
#define SHADER_PROGRAM_LIST_MAX 128


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

bool gf_draw_triangle(struct shader *shader, struct triangle_obj *triangle) {
  glUseProgram(shader->program);
  glBindVertexArray(triangle->vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  return true;
}

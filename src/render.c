#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <wayland-util.h>

#include "common.h"
#include "gf_math.h"
#include "log.h"
#include "render.h"

#define OBJ_LIST_MAX            128
#define SHADER_PROGRAM_LIST_MAX 128

static const GLuint BOX_INDEX_ORDER[] = {0, 1, 3, 0, 2, 3};

struct viewport_dimensions {
  int height;
  int width;
};

// Shared Global State shared by shaders.
struct render_state {
  struct viewport_dimensions viewport;
};

struct render_state render_state = {
    .viewport =
        {
            .height = GF_DEFAULT_WINDOW_HEIGHT,
            .width  = GF_DEFAULT_WINDOW_WIDTH,
        },
};

struct shader {
  GLuint vert;
  GLuint frag;
  GLuint program;
  struct shader_state {
    struct viewport_dimensions last_committed_viewport;
    struct transform {
      float scale_x;
      float scale_y;
      bool dirty;
    } transform;
  } state;
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

struct obj {
  GLuint vbo;
  GLuint vao;
  GLuint ebo;
  struct shader *shader;
};

struct obj_list {
  int count;
  int capacity;
  struct obj objs[OBJ_LIST_MAX];
};

static struct obj_list obj_list = {
    .count    = 0,
    .capacity = OBJ_LIST_MAX,
};

bool gf_draw_update_window_size(int32_t width, int32_t height) {
  if (height == render_state.viewport.height &&
      width == render_state.viewport.width) {
    return false;
  }
  render_state.viewport =
      (struct viewport_dimensions){.width = width, .height = height};
  return true;
}

void gf_shader_sync_projection_matrix(struct shader *shader) {
  int h = shader->state.last_committed_viewport.height;
  int w = shader->state.last_committed_viewport.width;

  gf_log(INFO_LOG,
         "Syncing projection matrix (h: %i, w: %i) to shader program '%i'.", h,
         w, shader->program);

  assert(h > 0);
  assert(w > 0);
  mat4 perspective_matrix;
  gf_ortho(0.0f, w, 0, h, -1.0, 1.0, perspective_matrix);

  glProgramUniformMatrix4fv(shader->program, GF_UNIFORM_PROJECTION_MAT_LOCATION,
                            1, false, (GLfloat *)perspective_matrix);
}

void gf_shader_sync_transform(struct shader *shader) {
  gf_log(INFO_LOG,
         "Syncing transformations (scale: { x: '%i', y: '%i'}) to shader "
         "program '%i'.",
         shader->state.transform.scale_x, shader->state.transform.scale_y,
         shader->program);
  mat2 transformation_matrix;
  gf_scale_mat2(shader->state.transform.scale_x,
                shader->state.transform.scale_y, transformation_matrix);
  glProgramUniformMatrix2fv(shader->program, GF_UNIFORM_TRANSFORM_MAT_LOCATION,
                            1, false, (GLfloat *)transformation_matrix);
}

void gf_shader_commit_state(struct shader *shader) {
  // Only sync if shader viewport out of sync with render state.
  struct viewport_dimensions *last_vp = &shader->state.last_committed_viewport,
                             *curr_vp = &render_state.viewport;
  if (last_vp->height != curr_vp->height || last_vp->width != curr_vp->width) {
    last_vp->height = curr_vp->height;
    last_vp->width  = curr_vp->width;
    gf_shader_sync_projection_matrix(shader);
  }
  if (shader->state.transform.dirty == true) {
    shader->state.transform.dirty = false;
    gf_shader_sync_transform(shader);
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

  shader->state = (struct shader_state){
      .last_committed_viewport =
          {
              .height = 0,
              .width  = 0,
          },
      .transform =
          {
              .scale_x = 3,
              .scale_y = 3,
              .dirty   = true,
          },
  };
  return shader;
}


struct obj *gf_obj_create_box(const struct box_verts *box_verts) {
  if (obj_list.count + 1 >= obj_list.capacity) {
    gf_log(DEBUG_LOG,
           "`box_obj_list` has a count of '%i', which is greater than "
           "it's capacity of '%i'.",
           obj_list.count, obj_list.capacity);
    return NULL;
  }

  struct obj *obj = &obj_list.objs[obj_list.count++];

  glCreateBuffers(1, &obj->vbo);
  glNamedBufferStorage(obj->vbo, sizeof(struct box_verts), box_verts,
                       GL_DYNAMIC_STORAGE_BIT);

  glCreateBuffers(1, &obj->ebo);
  glNamedBufferStorage(obj->ebo, sizeof(BOX_INDEX_ORDER), BOX_INDEX_ORDER,
                       GL_DYNAMIC_STORAGE_BIT);

  glCreateVertexArrays(1, &obj->vao);
  glVertexArrayVertexBuffer(obj->vao, 0, obj->vbo, 0, sizeof(struct vertex));
  glVertexArrayElementBuffer(obj->vao, obj->ebo);
  glEnableVertexArrayAttrib(obj->vao, 0);
  glVertexArrayAttribFormat(obj->vao, 0, 2, GL_FLOAT, false, 0);
  glVertexArrayAttribBinding(obj->vao, 0, 0);
  return obj;
}

bool gf_obj_set_shader(struct obj *obj, struct shader *shader) {
  if (obj->shader == shader) {
    return false;
  }
  obj->shader = shader;
  return true;
}

bool gf_obj_draw(struct obj *obj) {
  glUseProgram(obj->shader->program);
  glBindVertexArray(obj->vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  return true;
}

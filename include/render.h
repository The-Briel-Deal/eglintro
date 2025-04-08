#ifndef GF_DRAW_H
#define GF_DRAW_H


#include "gf_math.h"
#include <GL/gl.h>
#include <stdbool.h>
#include <stdint.h>

struct triangle_verts {
  vertex v1;
  vertex v2;
  vertex v3;
};

struct box_verts {
  vertex top_right;
  vertex top_left;
  vertex bottom_right;
  vertex bottom_left;
};

struct gf_obj {
  GLuint vbo;
  GLuint vao;
  GLuint ebo;
  struct obj_state {
    struct transform {
      tf_scale scale;
      tf_pos pos;
      radians rotation;
      bool dirty;
    } transform;
  } state;
  struct shader *shader;
};


bool gf_draw_update_window_size(int32_t height, int32_t width);
void gf_shader_commit_state(struct shader *shader);

struct gf_obj *gf_obj_create_box(const struct box_verts *box_verts);
bool gf_obj_set_shader(struct gf_obj *obj, struct shader *shader);

tf_scale gf_obj_get_scale(struct gf_obj *obj);
void gf_obj_set_scale(struct gf_obj *obj, tf_scale scale);

tf_pos gf_obj_get_pos(struct gf_obj *obj);
void gf_obj_set_pos(struct gf_obj *obj, tf_pos pos);

radians gf_obj_get_rotation(struct gf_obj *obj);
void gf_obj_set_rotation(struct gf_obj *obj, radians rotation);
void gf_obj_rotate_by(struct gf_obj *obj, radians rotation);

void gf_obj_commit_state(struct gf_obj *obj);
bool gf_obj_draw(struct gf_obj *obj);

struct shader *gf_compile_shaders(const char *vert_shader_src,
                                  const char *frag_shader_src);

#endif

#ifndef GF_DRAW_H
#define GF_DRAW_H


#include "gf_math.h"
#include <stdbool.h>
#include <stdint.h>

typedef vec2s vertex;

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
struct obj;

struct shader;

typedef vec2s tf_scale;
typedef vec2s tf_pos;

bool gf_draw_update_window_size(int32_t height, int32_t width);
void gf_shader_commit_state(struct shader *shader);

struct obj *gf_obj_create_box(const struct box_verts *box_verts);
bool gf_obj_set_shader(struct obj *obj, struct shader *shader);

tf_scale gf_obj_get_scale(struct obj *obj);
void gf_obj_set_scale(struct obj *obj, tf_scale scale);

tf_pos gf_obj_get_pos(struct obj *obj);
void gf_obj_set_pos(struct obj *obj, tf_pos pos);

void gf_obj_commit_state(struct obj *obj);
bool gf_obj_draw(struct obj *obj);

struct shader *gf_compile_shaders(const char *vert_shader_src,
                                  const char *frag_shader_src);

#endif

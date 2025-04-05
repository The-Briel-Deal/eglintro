#ifndef GF_DRAW_H
#define GF_DRAW_H


#include <stdbool.h>
#include <stdint.h>

struct vertex {
  float x;
  float y;
};

struct triangle_verts {
  struct vertex v1;
  struct vertex v2;
  struct vertex v3;
};
struct triangle_obj;

struct box_verts {
  struct vertex top_right;
  struct vertex top_left;
  struct vertex bottom_right;
  struct vertex bottom_left;
};
struct obj;

struct shader;

bool gf_draw_update_window_size(int32_t height, int32_t width);
void gf_shader_commit_state(struct shader *shader);

struct obj *gf_obj_create_box(const struct box_verts *box_verts);
bool gf_obj_set_shader(struct obj *obj, struct shader *shader);
void gf_obj_set_scale(struct obj *obj, const float x, const float y);
void gf_obj_commit_state(struct obj *obj);
bool gf_obj_draw(struct obj *obj);

struct shader *gf_compile_shaders(const char *vert_shader_src,
                                  const char *frag_shader_src);

#endif

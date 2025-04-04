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
struct box_obj;

struct shader;

bool gf_draw_update_window_size(int32_t height, int32_t width);
void gf_commit_render_state();

struct box_obj *gf_create_box(const struct box_verts *box_verts);
bool gf_set_box_shader(struct box_obj *box, struct shader *shader);
bool gf_draw_box(struct box_obj *box);


//! Creates a opengl triangle object based off the triangle_verts passed.
//! \param triangle_verts A pointer to the verticies the triangle will have.
//! \return An opaque pointer to the resulting triangle object.
struct triangle_obj *
gf_create_triangle(const struct triangle_verts *triangle_verts);
bool gf_set_triangle_shader(struct triangle_obj *triangle,
                            struct shader *shader);
bool gf_draw_triangle(struct triangle_obj *triangle);

struct shader *gf_compile_shaders(const char *vert_shader_src,
                                  const char *frag_shader_src);

#endif

#ifndef GF_DRAW_H
#define GF_DRAW_H


#include <stdbool.h>

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
struct shader;

//! Draw a box in the window, `x` is the percentage across the screen the top
//! left of the box starts at. Width is the percentage size of the box relative
//! to screen (e.g. 1.00 means the box is the width of the screen, and 0.50
//! means the box's width is half the screen width. `y` and `height` follow the
//! same principles.
bool gf_draw_box(float x, float y, float width, float height);

struct shader *gf_compile_shaders(const char *vert_shader_src,
                                  const char *frag_shader_src);

//! Creates a opengl triangle object based off the triangle_verts passed.
//! \param triangle_verts A pointer to the verticies the triangle will have.
//! \return An opaque pointer to the resulting triangle object.
struct triangle_obj *
gf_create_triangle(const struct triangle_verts *triangle_verts);

bool gf_set_triangle_shader(struct triangle_obj* triangle, struct shader* shader);

bool gf_draw_triangle(struct triangle_obj *triangle);

#endif

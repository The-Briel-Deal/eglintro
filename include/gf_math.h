#ifndef GF_MATH_H
#define GF_MATH_H
// NOTE:
// Much of this header is taken from the amazing library cglm.
// https://github.com/recp/cglm/tree/master
//
// I wasn't using a ton of stuff from the library so I just took and modified
// what I needed and put it in this header.

#include <string.h>

typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef vec2 mat2[2];
typedef vec3 mat3[3];
typedef vec4 mat4[4];

static inline void gf_mat4_zero(mat4 dest) {
  memset(dest, 0x0, sizeof(mat4));
}

static inline void gf_ortho(float left, float right, float bottom, float top,
                            float nearZ, float farZ, mat4 dest) {
  float rl, tb, fn;

  gf_mat4_zero(dest);

  rl = 1.0f / (right - left);
  tb = 1.0f / (top - bottom);
  fn = -1.0f / (farZ - nearZ);

  dest[0][0] = 2.0f * rl;
  dest[1][1] = 2.0f * tb;
  dest[2][2] = 2.0f * fn;
  dest[3][0] = -(right + left) * rl;
  dest[3][1] = -(top + bottom) * tb;
  dest[3][2] = (farZ + nearZ) * fn;
  dest[3][3] = 1.0f;
}

static inline void gf_mat2_zero(mat2 dest) {
  memset(dest, 0x0, sizeof(mat2));
}
static inline void gf_scale_mat2(float x, float y, mat2 dest) {
  gf_mat2_zero(dest);

  dest[0][0] = x;
  dest[1][1] = y;
}


#endif

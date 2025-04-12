#ifndef GF_MATH_H
#define GF_MATH_H
// NOTE:
// Much of this header is taken from the amazing library cglm.
// https://github.com/recp/cglm/tree/master
//
// I wasn't using a ton of stuff from the library so I just took and modified
// what I needed and put it in this header.

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

// Array Based Vector and Matrix.
typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef vec2 mat2[2];
typedef vec3 mat3[3];
typedef vec4 mat4[4];

// Struct Based Vector and Matrix.
typedef struct {
  float x;
  float y;
} vec2s;

typedef vec2s vertex;

typedef vec2s tf_scale;
typedef vec2s tf_pos;
typedef float radians;

static inline float gf_vec2s_magnitude(vec2s v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}

static inline void gf_vec2s_normalize(vec2s *v) {
  float mag = gf_vec2s_magnitude(*v);
  v->x      = v->x / mag;
  v->y      = v->y / mag;
  assert(gf_vec2s_magnitude(*v) > 0.9 && gf_vec2s_magnitude(*v) < 1.1);
}

static inline void gf_vec2s_scale(vec2s *v, float factor) {
  v->x *= factor;
  v->y *= factor;
}

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

static const mat3 MAT3_IDENTITY = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
};

static inline void gf_mat3_identity(mat3 dest) {
  memcpy(dest, MAT3_IDENTITY, sizeof(mat3));
}

static inline void gf_mat3_mul(mat3 m1, mat3 m2, mat3 dest) {
  float a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2];
  float a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2];
  float a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2];

  float b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2];
  float b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2];
  float b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2];

  dest[0][0] = a00 * b00 + a10 * b01 + a20 * b02;
  dest[0][1] = a01 * b00 + a11 * b01 + a21 * b02;
  dest[0][2] = a02 * b00 + a12 * b01 + a22 * b02;
  dest[1][0] = a00 * b10 + a10 * b11 + a20 * b12;
  dest[1][1] = a01 * b10 + a11 * b11 + a21 * b12;
  dest[1][2] = a02 * b10 + a12 * b11 + a22 * b12;
  dest[2][0] = a00 * b20 + a10 * b21 + a20 * b22;
  dest[2][1] = a01 * b20 + a11 * b21 + a21 * b22;
  dest[2][2] = a02 * b20 + a12 * b21 + a22 * b22;
}

static inline void gf_mat3_scale(tf_scale scale, mat3 dest) {
  dest[0][0] = scale.x;
  dest[1][1] = scale.y;
}

static inline void gf_mat3_translate(tf_pos pos, mat3 dest) {
  dest[2][0] = pos.x;
  dest[2][1] = pos.y;
}

static inline void gf_mat3_print(mat3 mat, char *name) {
  printf("\nMat '%s':\n", name);
  printf("%f %f %f\n", mat[0][0], mat[1][0], mat[2][0]);
  printf("%f %f %f\n", mat[0][1], mat[1][1], mat[2][1]);
  printf("%f %f %f\n", mat[0][2], mat[1][2], mat[2][2]);
}

static const mat4 MAT4_IDENTITY = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
};

static inline void gf_mat4_identity(mat4 dest) {
  memcpy(dest, MAT4_IDENTITY, sizeof(mat4));
}
static inline void gf_mat4_mul(mat4 m1, mat4 m2, mat4 dest) {
  float a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3];
  float a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3];
  float a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3];
  float a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3];

  float b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2], b03 = m2[0][3];
  float b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2], b13 = m2[1][3];
  float b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2], b23 = m2[2][3];
  float b30 = m2[3][0], b31 = m2[3][1], b32 = m2[3][2], b33 = m2[3][3];

  dest[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
  dest[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
  dest[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
  dest[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
  dest[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
  dest[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
  dest[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
  dest[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
  dest[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
  dest[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
  dest[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
  dest[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
  dest[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
  dest[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
  dest[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
  dest[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
}

static inline void gf_mat4_scale2d_new(tf_scale scale, mat4 dest) {
  gf_mat4_identity(dest);
  dest[0][0] = scale.x;
  dest[1][1] = scale.y;
}

static inline void gf_mat4_scale2d(mat4 m, tf_scale scale) {
  m[0][0] = m[0][0] * scale.x;
  m[0][1] = m[0][1] * scale.x;
  m[0][2] = m[0][2] * scale.x;

  m[1][0] = m[1][0] * scale.y;
  m[1][1] = m[1][1] * scale.y;
  m[1][2] = m[1][2] * scale.y;
}


static inline void gf_mat4_translate2d_new(tf_pos pos, mat4 dest) {
  gf_mat4_identity(dest);
  dest[3][0] = pos.x;
  dest[3][1] = pos.y;
}

// TODO: Look at cglm for a better way to do this.
static inline void gf_mat4_translate2d(mat4 m, tf_pos pos) {
  mat4 temp;
  gf_mat4_translate2d_new(pos, temp);
  gf_mat4_mul(m, temp, m);
}


static inline void gf_mat4_rotate2d(mat4 m, float angle) {
  float m00 = m[0][0], m10 = m[1][0];
  float m01 = m[0][1], m11 = m[1][1];
  float m02 = m[0][2], m12 = m[1][2];
  float c, s;

  s = sinf(angle);
  c = cosf(angle);

  m[0][0] = m00 * c + m10 * s;
  m[0][1] = m01 * c + m11 * s;
  m[0][2] = m02 * c + m12 * s;

  m[1][0] = m00 * -s + m10 * c;
  m[1][1] = m01 * -s + m11 * c;
  m[1][2] = m02 * -s + m12 * c;
}


static inline void gf_mat4_print(mat4 mat, char *name) {
  printf("\nMat '%s':\n", name);
  printf("%f %f %f %f\n", mat[0][0], mat[1][0], mat[2][0], mat[3][0]);
  printf("%f %f %f %f\n", mat[0][1], mat[1][1], mat[2][1], mat[3][1]);
  printf("%f %f %f %f\n", mat[0][2], mat[1][2], mat[2][2], mat[3][2]);
  printf("%f %f %f %f\n", mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
}

#endif

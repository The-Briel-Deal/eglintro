#include "gf_math.h"
#include "test_common.h"

static void test_gf_vec2s_scale() {
  vec2s v = {.x = 1.0, .y = 1.0};
  gf_vec2s_scale(&v, 2.0);
  ASSERT_NEAR(v.x, 2.0);
  ASSERT_NEAR(v.y, 2.0);
}
static void test_gf_vec2s_normalize() {
  vec2s v = {.x = 1.0, .y = 1.0};
  gf_vec2s_normalize(&v);
  ASSERT_NEAR(v.x, 0.707106765f);
  ASSERT_NEAR(v.y, 0.707106765f);
}

static void test_gf_vec2s_lerp() {
  vec2s from = {.x = 1.0, .y = 0.0};
  vec2s to   = {.x = 0.0, .y = 1.0};
  vec2s result;
  gf_vec2s_lerp(&from, &to, 0.5, &result);

  ASSERT_NEAR(result.x, 0.50);
  ASSERT_NEAR(result.y, 0.50);
}

void run_gf_math_tests() {
  RUN_TEST(test_gf_vec2s_scale);
  RUN_TEST(test_gf_vec2s_normalize);
  RUN_TEST(test_gf_vec2s_lerp);
}

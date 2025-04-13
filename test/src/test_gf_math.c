#include "test_common.h"
#include "gf_math.h"

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

void run_gf_math_tests() {
  RUN_TEST(test_gf_vec2s_scale);
  RUN_TEST(test_gf_vec2s_normalize);
}

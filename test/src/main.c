#include <stdbool.h>
#include <stdio.h>

#include "gf_math.h"

#define RUN_TEST(test)                                                         \
  do {                                                                         \
    printf("Running Test " #test "\n");                                        \
    test();                                                                    \
  } while (false)

#define FLOAT_ACCEPTABLE_VARIANCE 0.0000001f

#define ASSERT_NEAR(result, expect_near)                                       \
  do {                                                                         \
    if (result > expect_near + FLOAT_ACCEPTABLE_VARIANCE) {                    \
      char msg[128];                                                           \
      sprintf(msg, "Result '%f' is greater than expectation '%f'.", result,    \
              expect_near);                                                    \
      __assert_fail(msg, __FILE__, __LINE__, __ASSERT_FUNCTION);               \
    }                                                                          \
    if (result < expect_near - FLOAT_ACCEPTABLE_VARIANCE) {                    \
      char msg[128];                                                           \
      sprintf(msg, "Result '%f' is less than expectation '%f'.", result,       \
              expect_near);                                                    \
      __assert_fail(msg, __FILE__, __LINE__, __ASSERT_FUNCTION);               \
    }                                                                          \
  } while (false)

void test_gf_vec2s_scale() {
  vec2s v = {.x = 1.0, .y = 1.0};
  gf_vec2s_scale(&v, 2.0);
  ASSERT_NEAR(v.x, 2.0);
  ASSERT_NEAR(v.y, 2.0);
}
void test_gf_vec2s_normalize() {
  vec2s v = {.x = 1.0, .y = 1.0};
  gf_vec2s_normalize(&v);
  ASSERT_NEAR(v.x, 0.707106765f);
  ASSERT_NEAR(v.y, 0.707106765f);
}

int main() {
  RUN_TEST(test_gf_vec2s_scale);
  RUN_TEST(test_gf_vec2s_normalize);
}

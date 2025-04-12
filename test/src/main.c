
#include "gf_math.h"
#include <stdbool.h>

#define RUN_TEST(test)                                                         \
  do {                                                                         \
    printf("Running Test " #test "\n");                                        \
    test();                                                                    \
  } while (false)

void test_gf_vec2s_scale() {
  vec2s v = {.x = 1.0, .y = 1.0};
  gf_vec2s_scale(&v, 2.0);
  assert(v.x == 2.0);
  assert(v.y == 2.0);
}

int main() {
  RUN_TEST(test_gf_vec2s_scale);
}

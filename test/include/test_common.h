#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <stdbool.h>
#include <stdio.h>

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

#endif

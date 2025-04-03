#ifndef GF_LOG_H
#define GF_LOG_H

#include <stdarg.h>
#include <stdio.h>
#ifndef LOG_LEVEL
#define LOG_LEVEL 2
#endif

enum log {
  INFO_LOG  = 0,
  DEBUG_LOG = 1,
  ERROR_LOG = 2,
};

static inline void gf_log(enum log severity, char *fmt_msg, ...) {
  va_list args;
  va_start(args, fmt_msg);
  if (severity >= LOG_LEVEL) {
    switch (severity) {
      case INFO_LOG : printf("INFO  - "); break;
      case DEBUG_LOG: printf("DEBUG - "); break;
      case ERROR_LOG: printf("ERROR - "); break;
    }
    vprintf(fmt_msg, args);
  }
  va_end(args);
	printf("\n");
}

#endif

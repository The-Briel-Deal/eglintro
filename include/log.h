#ifndef GF_LOG_H
#define GF_LOG_H

#include <stdarg.h>
#include <stdio.h>

#define ANSI_COLOR_RED    "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_RESET  "\x1b[0m"

#ifndef LOG_LEVEL
#define LOG_LEVEL 2
#endif


enum log_severity {
  INFO_LOG  = 0,
  DEBUG_LOG = 1,
  ERROR_LOG = 2,
};

static inline void gf_log(enum log_severity severity, char *fmt_msg, ...) {
  if (severity >= LOG_LEVEL) {
    switch (severity) {
      case INFO_LOG : printf("INFO  - "); break;
      case DEBUG_LOG: printf(ANSI_COLOR_YELLOW "DEBUG - "); break;
      case ERROR_LOG: printf(ANSI_COLOR_RED "ERROR - "); break;
    }
    va_list args;
    va_start(args, fmt_msg);
    vprintf(fmt_msg, args);
    va_end(args);
    printf(ANSI_COLOR_RESET "\n");
  }
}

#endif

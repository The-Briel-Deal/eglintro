#ifndef GF_LOG_H
#define GF_LOG_H

#include <stdio.h>
#ifndef LOG_LEVEL
#define LOG_LEVEL 2
#endif

enum log {
  INFO_LOG  = 0,
  DEBUG_LOG = 1,
  ERROR_LOG = 2,
};

void gf_log(enum log severity, char *msg) {
  if (severity >= LOG_LEVEL) {
    switch (severity) {
      case INFO_LOG : printf("INFO  - "); break;
      case DEBUG_LOG: printf("DEBUG - "); break;
      case ERROR_LOG: printf("ERROR - "); break;
    }
    printf("%s\n", msg);
  }
}

#endif

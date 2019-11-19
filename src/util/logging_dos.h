#ifndef UTIL_LOGGING_H_
#define UTIL_LOGGING_H_

#include <cstdio>

#include "io/vga_dos.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define ASSERT(a)                                    \
  do {                                               \
    if (!(a)) {                                      \
      vga::reset();                                  \
      printf("Assertion Failed: " TOSTRING(a) "\n"); \
      exit(1);                                       \
    }                                                \
  } while (0)

#endif
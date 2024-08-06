#include "imm_clock.h"

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <time.h>

#if defined(__APPLE__)
#define IMM_USE_CLOCKID CLOCK_REALTIME
#elif defined(CLOCK_MONOTONIC)
#define IMM_USE_CLOCKID CLOCK_MONOTONIC
#elif defined(CLOCK_REALTIME)
#define IMM_USE_CLOCKID CLOCK_REALTIME
#else
#error "Cannot get time from clock"
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long imm_clock(void)
{
  struct timespec ts;

  if (clock_gettime(IMM_USE_CLOCKID, &ts) != 0)
  {
    fprintf(stderr, "clock_gettime failed: %s", strerror(errno));
    exit(1);
  }

  long msec = ts.tv_sec;
  msec *= 1000;
  msec += (ts.tv_nsec / 1000000);
  return msec;
}

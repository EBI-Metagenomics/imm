#ifndef LOG_H
#define LOG_H

#include "imm/imm.h"

#if defined(HAVE_ATTR_FMT)
#define ATTR_FMT __attribute__((format(printf, 4, 5)))
#else
#define ATTR_FMT
#endif

void log_log(enum imm_log_level level, char const* file, int line, char const* fmt, ...) ATTR_FMT;

#endif

#ifndef LOG_H
#define LOG_H

#include "imm/imm.h"
#include <stdarg.h>

#if defined(HAVE_ATTR_FMT)
#define ATTR_FMT __attribute__((format(printf, 4, 5)))
#else
#define ATTR_FMT
#endif

#define warn(...) log_log(IMM_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define error(...) log_log(IMM_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define fatal(...) log_log(IMM_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

char const* explain(enum imm_error_code ecode);
void        log_log(enum imm_log_level level, char const* file, int line, char const* fmt, ...) ATTR_FMT;

#endif

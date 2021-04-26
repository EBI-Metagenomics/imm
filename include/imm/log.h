#ifndef IMM_LOG_H
#define IMM_LOG_H

#include "imm/export.h"
#include <stdarg.h>

enum imm_log_level
{
    IMM_LOG_TRACE,
    IMM_LOG_DEBUG,
    IMM_LOG_INFO,
    IMM_LOG_WARN,
    IMM_LOG_ERROR,
    IMM_LOG_FATAL
};

struct imm_log_event
{
    va_list va;
    char const *fmt;
    char const *file;
    int line;
    enum imm_log_level level;
};

typedef void (*imm_log_callback)(struct imm_log_event event);

IMM_API void imm_log_setup(imm_log_callback cb, enum imm_log_level level);

#define imm_log_warn(...)                                                      \
    __imm_log(IMM_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)

#define imm_log_error(...)                                                     \
    __imm_log(IMM_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#define imm_log_fatal(...)                                                     \
    __imm_log(IMM_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#if defined(HAVE_ATTR_FMT)
IMM_API int __imm_log(enum imm_log_level level, char const *file, int line,
                      char const *fmt, ...)
    __attribute__((format(printf, 4, 5)));
#else
IMM_API int __imm_log(enum imm_log_level level, char const *file, int line,
                      char const *fmt, ...);
#endif

#endif

#ifndef IMM_LOG_H
#define IMM_LOG_H

#include "imm/export.h"
#include <stdarg.h>

enum imm_level
{
    IMM_TRACE,
    IMM_DEBUG,
    IMM_INFO,
    IMM_WARN,
    IMM_ERROR,
    IMM_FATAL
};

enum imm_code
{
    IMM_SUCCESS = 0,
    IMM_FAILURE,
    IMM_OUTOFMEM,
    IMM_ILLEGALARG,
    IMM_IOERROR,
    IMM_NOTIMPLEMENTED,
    IMM_RUNTIMEERROR,
    IMM_PARSEERROR,
};

struct imm_log_event
{
    va_list va;
    char const *fmt;
    char const *file;
    int line;
    enum imm_level level;
    enum imm_code code;
};

typedef void (*imm_log_callback)(struct imm_log_event event);

IMM_API void imm_log_default_callback(struct imm_log_event event);
IMM_API void imm_log_setup(imm_log_callback cb, enum imm_level level);

#define __imm_log(lvl, code, ...)                                              \
    __imm_log_impl(lvl, code, __FILE__, __LINE__, __VA_ARGS__)

IMM_API int __imm_log_impl(enum imm_level level, enum imm_code code,
                           char const *file, int line, char const *fmt, ...)
    __attribute__((format(printf, 5, 6)));

#ifdef NDEBUG
#define IMM_BUG(cond)
#else
#define IMM_BUG(cond)                                                          \
    do                                                                         \
    {                                                                          \
        if (!(cond))                                                           \
            break;                                                             \
        __imm_bug(__FILE__, __LINE__, #cond);                                  \
    } while (0)
#endif

IMM_API void __imm_bug(char const *file, int line, char const *cond)
    __attribute__((noreturn));

#endif

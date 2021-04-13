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
    va_list            params;
    char const*        fmt;
    char const*        file;
    int                line;
    enum imm_log_level level;
};

typedef void (*imm_log_callback)(struct imm_log_event event);

IMM_API void imm_log_setup(imm_log_callback cb, enum imm_log_level level);

#endif

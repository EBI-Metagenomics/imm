#ifndef COMMON_ERROR_H
#define COMMON_ERROR_H

#include "imm/error.h"
#include "imm/log.h"

#define warn(...) imm_log_warn(__VA_ARGS__)
#define error(...) imm_log_error(__VA_ARGS__)
#define fatal(...) imm_log_fatal(__VA_ARGS__)

#define xerror(err, msg) __xerror(err, msg, __FILE__, __LINE__)

int __xerror(enum imm_error_code code, char const *msg, char const *file,
             int line);

#endif

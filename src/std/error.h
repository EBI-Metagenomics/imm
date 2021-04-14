#ifndef STD_ERROR_H
#define STD_ERROR_H

#include "imm/imm.h"
#include "log.h"

#define warn(...) log_log(IMM_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define error(...) log_log(IMM_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define fatal(...) log_log(IMM_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#define error_explain(ERROR) error("%s", explain((ERROR)))

char const* explain(enum imm_error_code ecode);

#endif

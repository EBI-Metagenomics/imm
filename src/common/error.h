#ifndef COMMON_ERROR_H
#define COMMON_ERROR_H

#include "imm/error.h"
#include "imm/log.h"

#define warn(...) imm_log_warn(__VA_ARGS__)
#define error(...) imm_log_error(__VA_ARGS__)
#define fatal(...) imm_log_fatal(__VA_ARGS__)

#define error_explain(ERROR) error("%s", explain((ERROR)))

char const *explain(enum imm_error_code ecode);

#endif

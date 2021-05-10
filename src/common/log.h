#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include "imm/log.h"

#define warn(code, ...) __imm_log(IMM_WARN, code, __VA_ARGS__)
#define error(code, ...) __imm_log(IMM_ERROR, code, __VA_ARGS__)
#define fatal(code, ...) __imm_log(IMM_FATAL, code, __VA_ARGS__)

#endif

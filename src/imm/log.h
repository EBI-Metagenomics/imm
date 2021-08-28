#ifndef IMM_LOG_H
#define IMM_LOG_H

#include "imm/export.h"
#include "imm/rc.h"

typedef void imm_log_error_t(char const *msg, void *arg);
typedef void imm_log_fatal_t(char const *msg, void *arg);
typedef void imm_log_flush_t(void *arg);

IMM_API void imm_log_setup(imm_log_error_t *error, imm_log_fatal_t *fatal,
                           imm_log_flush_t *flush, void *arg);

#endif

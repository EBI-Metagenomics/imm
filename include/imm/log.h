#ifndef IMM_LOG_H
#define IMM_LOG_H

#include "imm/export.h"

typedef void imm_log_print_t(char const *msg, void *arg);

IMM_API void imm_log_setup(imm_log_print_t *print, void *arg);

#endif

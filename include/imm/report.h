#ifndef IMM_REPORT_H_API
#define IMM_REPORT_H_API

#include "imm/api.h"
#include <stdarg.h>

IMM_API void imm_error(const char *err, ...) __attribute__((format(printf, 1, 2)));

#endif

#ifndef IMM_REPORT_H
#define IMM_REPORT_H

#include "imm/api.h"
#include <stdarg.h>

IMM_API void imm_error(char const* err, ...) __attribute__((format(printf, 1, 2)));

#endif

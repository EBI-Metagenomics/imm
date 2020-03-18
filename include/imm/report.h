#ifndef IMM_REPORT_H
#define IMM_REPORT_H

#include "imm/export.h"
#include <stdarg.h>

#if defined(HAVE_ATTR_FORMAT)
#define ATTR_FORMAT __attribute__((format(printf, 1, 2)))
#else
#define ATTR_FORMAT
#endif

IMM_EXPORT void imm_error(char const* err, ...) ATTR_FORMAT;
IMM_EXPORT void imm_die(char const* err, ...) ATTR_FORMAT;

#endif

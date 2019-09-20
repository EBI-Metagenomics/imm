#ifndef _NHMM_REPORT_H
#define _NHMM_REPORT_H

#include <stdarg.h>

void error(const char *err, ...) __attribute__((format (printf, 1, 2)));

#endif

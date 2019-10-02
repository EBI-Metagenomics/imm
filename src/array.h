#ifndef IMM_ARRAY_H
#define IMM_ARRAY_H

#include <stddef.h>

double logsumexp(double *arr, size_t len);
int log_normalize(double *arr, size_t len);

#endif

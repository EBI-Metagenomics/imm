#ifndef IMM_STRLCPY_H
#define IMM_STRLCPY_H

#include "compiler.h"
#include <stddef.h>

size_t imm_strlcpy(char *dst, char const *src, size_t dsize);

#endif

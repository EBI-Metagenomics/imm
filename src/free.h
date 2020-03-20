#ifndef IMM_FREE_H
#define IMM_FREE_H

#include <stdlib.h>

static inline void imm_free(void const* p) { free((void*)p); }

#endif

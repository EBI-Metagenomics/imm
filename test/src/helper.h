#ifndef HELPER_H
#define HELPER_H

#include "imm/imm.h"

static inline char* fmt_name(char* restrict buffer, char const* name, int i)
{
    sprintf(buffer, "%s%d", name, i);
    return buffer;
}
static inline imm_float zero(void) { return imm_lprob_zero(); }

#endif

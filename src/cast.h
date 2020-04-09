#ifndef CAST_H
#define CAST_H

#include "imm/bug.h"
#include <inttypes.h>
#include <limits.h>

static inline uint8_t cast_u8_u(unsigned src)
{
    IMM_BUG(src > UINT8_MAX);
    return (uint8_t)src;
}

static inline unsigned cast_u_lu(unsigned long src)
{
    IMM_BUG(src > UINT_MAX);
    return (unsigned)src;
}

static inline uint32_t cast_u32_u(unsigned src)
{
    IMM_BUG(src > UINT32_MAX);
    return (uint32_t)src;
}

#endif

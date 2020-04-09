#ifndef CAST_H
#define CAST_H

#include "imm/bug.h"
#include <inttypes.h>
#include <limits.h>
#include <stddef.h>

static inline uint8_t cast_u8_u(unsigned src)
{
    IMM_BUG(src > UINT8_MAX);
    return (uint8_t)src;
}

static inline uint32_t cast_u32_u(unsigned src)
{
    IMM_BUG(src > UINT32_MAX);
    return (uint32_t)src;
}

static inline unsigned cast_u_lu(unsigned long src)
{
    IMM_BUG(src > UINT_MAX);
    return (unsigned)src;
}

static inline uint16_t cast_u16_zu(size_t src)
{
    IMM_BUG(src > UINT16_MAX);
    return (uint16_t)src;
}

static inline unsigned cast_u_zu(size_t src)
{
    IMM_BUG(src > UINT_MAX);
    return (unsigned)src;
}

#endif

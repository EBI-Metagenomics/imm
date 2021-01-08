#ifndef CAST_H
#define CAST_H

#include "imm/bug.h"
#include <inttypes.h>
#include <limits.h>
#include <stddef.h>

static inline unsigned cast_i_u(int src);
static inline uint32_t cast_u_u32(unsigned src);
static inline uint16_t cast_u_u16(unsigned src);
static inline uint8_t  cast_u_u8(unsigned src);
static inline unsigned cast_ul_u(unsigned long src);
static inline uint32_t cast_ul_u32(unsigned long src);
static inline uint16_t cast_ul_u16(unsigned long src);
static inline unsigned cast_zu_u(size_t src);
static inline uint16_t cast_zu_u16(size_t src);
static inline uint8_t  cast_zu_u8(size_t src);

static inline unsigned cast_i_u(int src)
{
    IMM_BUG(src < 0);
    return (unsigned)src;
}

static inline uint32_t cast_u_u32(unsigned src)
{
    IMM_BUG(src > UINT32_MAX);
    return (uint32_t)src;
}

static inline uint16_t cast_u_u16(unsigned src)
{
    IMM_BUG(src > UINT16_MAX);
    return (uint16_t)src;
}

static inline uint8_t cast_u_u8(unsigned src)
{
    IMM_BUG(src > UINT8_MAX);
    return (uint8_t)src;
}

static inline unsigned cast_ul_u(unsigned long src)
{
    IMM_BUG(src > UINT_MAX);
    return (unsigned)src;
}

static inline uint32_t cast_ul_u32(unsigned long src)
{
    IMM_BUG(src > UINT32_MAX);
    return (uint32_t)src;
}

static inline uint16_t cast_ul_u16(unsigned long src)
{
    IMM_BUG(src > UINT16_MAX);
    return (uint16_t)src;
}

static inline unsigned cast_zu_u(size_t src)
{
    IMM_BUG(src > UINT_MAX);
    return (unsigned)src;
}

static inline uint16_t cast_zu_u16(size_t src)
{
    IMM_BUG(src > UINT16_MAX);
    return (uint16_t)src;
}

static inline uint8_t cast_zu_u8(size_t src)
{
    IMM_BUG(src > UINT8_MAX);
    return (uint8_t)src;
}

#endif

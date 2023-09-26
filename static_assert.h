#ifndef IMM_STATIC_ASSERT_H
#define IMM_STATIC_ASSERT_H

#include <assert.h>

#ifdef static_assert
#define imm_static_assert(expr, msg) static_assert(expr, msg)
#else
#define imm_static_assert(expr, msg) _Static_assert(expr, msg)
#endif

#endif

#ifndef IMM_UNREACHABLE_H
#define IMM_UNREACHABLE_H

#include "compiler.h"

#if __has_builtin(__builtin_unreachable)
#define imm_unreachable() __builtin_unreachable()
#else
#define imm_unreachable()
#endif

#endif

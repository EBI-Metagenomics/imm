#ifndef IMM_TEST_HELPER_H
#define IMM_TEST_HELPER_H

#include "vendor/minctest.h"

#define eq(a, b) lequal(a, b, "%d")
#define eqllu(a, b) lequal(a, b, "%llu")
#define close(a, b) lfequal(a, b)
#define ok(x) lok(x)

#endif

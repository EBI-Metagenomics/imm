#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define STRUCT_FIELD(s, f) (((struct s *)0)->f)

#include "common/bits.h"
#include "common/io.h"
#include "common/log.h"
#include "common/math.h"
#include "common/memory.h"
#include "common/string.h"

#endif

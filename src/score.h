#ifndef SCORE_H
#define SCORE_H

#include <stdint.h>

typedef uint64_t score_t;
#define SCORE_MIN ((size_t)0)
#define SCORE_MAX ((size_t)(UINT64_MAX - 1))
#define SCORE_INV ((size_t)(UINT64_MAX))

#endif

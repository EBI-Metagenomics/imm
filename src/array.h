#ifndef IMM_ARRAY_H
#define IMM_ARRAY_H

#include "free.h"
#include <stdlib.h>

#define MAKE_ARRAY_STRUCT(S, T)                                                               \
    struct array_##S                                                                          \
    {                                                                                         \
        T*  elements;                                                                         \
        int length;                                                                           \
    };

#define MAKE_ARRAY_INIT(S)                                                                    \
    static inline void array_##S##_init(struct array_##S* array)                              \
    {                                                                                         \
        array->elements = NULL;                                                               \
        array->length = 0;                                                                    \
    }

#define MAKE_ARRAY_APPEND(S, T)                                                               \
    static inline T* array_##S##_append(struct array_##S* array)                              \
    {                                                                                         \
        array->elements = realloc(array->elements, ((size_t)(++array->length)) * sizeof(T));  \
        return array->elements + array->length - 1;                                           \
    }

#define MAKE_ARRAY_GET(S, T)                                                                  \
    static inline T* array_##S##_get(struct array_##S* array, int idx)                        \
    {                                                                                         \
        return array->elements + idx;                                                         \
    }

#define MAKE_ARRAY_GET_C(S, T)                                                                \
    static inline T const* array_##S##_get_c(struct array_##S const* array, int idx)          \
    {                                                                                         \
        return array->elements + idx;                                                         \
    }

#define MAKE_ARRAY_LENGTH(S)                                                                  \
    static inline int array_##S##_length(struct array_##S const* array)                       \
    {                                                                                         \
        return array->length;                                                                 \
    }

#define MAKE_ARRAY_EMPTY(S)                                                                   \
    static inline void array_##S##_empty(struct array_##S* array)                             \
    {                                                                                         \
        free_c(array->elements);                                                              \
        array->elements = NULL;                                                               \
        array->length = 0;                                                                    \
    }

#endif

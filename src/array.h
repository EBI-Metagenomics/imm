#ifndef ARRAY_H
#define ARRAY_H

#include "free.h"
#include <stdlib.h>

#define MAKE_ARRAY_STRUCT(S, T)                                                               \
    struct array_##S                                                                          \
    {                                                                                         \
        T*       elements;                                                                    \
        unsigned length;                                                                      \
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
    static inline T* array_##S##_get(struct array_##S* array, unsigned const idx)             \
    {                                                                                         \
        return array->elements + idx;                                                         \
    }

#define MAKE_ARRAY_GET_C(S, T)                                                                \
    static inline T const* array_##S##_get_c(struct array_##S const* array,                   \
                                             unsigned const          idx)                     \
    {                                                                                         \
        return array->elements + idx;                                                         \
    }

#define MAKE_ARRAY_LENGTH(S)                                                                  \
    static inline unsigned array_##S##_length(struct array_##S const* array)                  \
    {                                                                                         \
        return array->length;                                                                 \
    }

#define MAKE_ARRAY_EMPTY(S)                                                                   \
    static inline void array_##S##_empty(struct array_##S* array)                             \
    {                                                                                         \
        imm_free(array->elements);                                                              \
        array->elements = NULL;                                                               \
        array->length = 0;                                                                    \
    }

#endif

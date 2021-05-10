#ifndef COMMON_IO_H
#define COMMON_IO_H

#include "common/log.h"
#include <stdio.h>

#define xfwrite(buffer, size, count, stream)                                   \
    __fwrite((buffer), (size), (count), (stream), __FILE__, __LINE__, #buffer)

static inline void __fwrite(const void *restrict buffer, size_t size,
                            size_t count, FILE *restrict stream,
                            char const *file, int line, char const *buffstr)
{
    if (fwrite(buffer, size, count, stream) < count)
        error(IMM_IOERROR, "failed to write %s", buffstr);
}

#endif

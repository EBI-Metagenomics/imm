#ifndef STD_IO_H
#define STD_IO_H

#include <stdio.h>

#define xfwrite(buffer, size, count, stream)                                                                           \
    std_fwrite((buffer), (size), (count), (stream), __FILE__, __LINE__, #buffer)

void std_fwrite(const void* restrict buffer, size_t size, size_t count, FILE* restrict stream, char const* file,
                int line, char const* buffstr);

#endif

#ifndef STD_MEMORY_H
#define STD_MEMORY_H

#include <stddef.h>

#define xmalloc(x) std_malloc((x), __FILE__, __LINE__)
#define xstrdup(x) std_strdup((x), __FILE__, __LINE__)

void free_if(void const* ptr);

void* std_malloc(size_t size, char const* file, int line);
char* std_strdup(char const* str, char const* file, int line);

#endif

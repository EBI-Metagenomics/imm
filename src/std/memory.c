#include "std/memory.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>

void free_if(void const* ptr)
{
    if (ptr)
        free((void*)ptr);
}

void* std_malloc(size_t size, char const* file, int line)
{
    void* ptr = malloc(size);
    if (!ptr) {
        log_log(IMM_LOG_FATAL, file, line, "failed to malloc");
        exit(1);
    }
    return ptr;
}

void* std_memcpy(void* restrict dest, const void* restrict src, size_t count, char const* file, int line)
{
    void* ptr = memcpy(dest, src, count);
    if (!ptr) {
        log_log(IMM_LOG_FATAL, file, line, "failed to memcpy");
        exit(1);
    }
    return ptr;
}

char* std_strdup(char const* str, char const* file, int line)
{
    char* new = strdup(str);
    if (!new) {
        log_log(IMM_LOG_FATAL, file, line, "failed to strdup");
        exit(1);
    }
    return new;
}

#include "std/io.h"
#include "log.h"
#include <stdlib.h>

void std_fwrite(const void* restrict buffer, size_t size, size_t count, FILE* restrict stream, char const* file,
                int line, char const* buffstr)
{
    if (fwrite(buffer, size, count, stream) < count)
        log_log(IMM_LOG_ERROR, file, line, "failed to write %s", buffstr);
}

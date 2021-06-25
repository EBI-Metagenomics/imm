#include "imm/imm.h"
#include <stdio.h>

extern int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FILE *file = fmemopen((void *)data, size, "r");
    if (!file)
        return 0;
    struct imm_abc abc_in = imm_abc_empty;
    imm_abc_read(&abc_in, file);
    fclose(file);
    return 0;
}

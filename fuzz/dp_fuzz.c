#include "imm/imm.h"
#include <stdio.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    imm_log_setup(imm_log_default_callback, IMM_FATAL);

    FILE *file = fmemopen((void *)data, size, "r");
    if (!file)
        return 0;
    struct imm_abc abc = imm_abc_empty;
    struct imm_dp *dp = imm_dp_new(&abc);
    imm_dp_read(dp, file);
    imm_del(dp);
    fclose(file);
    return 0;
}

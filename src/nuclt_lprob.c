#include "imm/nuclt_lprob.h"
#include "error.h"
#include "imm/rc.h"
#include "io.h"

enum imm_rc imm_nuclt_lprob_write(struct imm_nuclt_lprob const *nucltp,
                                  FILE *file)
{
    cmp_ctx_t ctx = {0};
    io_init(&ctx, file);

    for (unsigned i = 0; i < IMM_NUCLT_SIZE; ++i)
    {
        if (!io_write_imm_float(&ctx, nucltp->lprobs[i]))
            return error(IMM_IOERROR, "failed to write imm_float");
    }
    return IMM_SUCCESS;
}

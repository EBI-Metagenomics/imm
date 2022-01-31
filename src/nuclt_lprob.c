#include "imm/nuclt_lprob.h"
#include "error.h"
#include "imm/rc.h"
#include "io.h"
#include "xcmp.h"

enum imm_rc imm_nuclt_lprob_write(struct imm_nuclt_lprob const *nucltp,
                                  FILE *file)
{
    cmp_ctx_t ctx = {0};
    xcmp_fsetup(&ctx, file);

    cmp_write_array(&ctx, IMM_NUCLT_SIZE);
    for (unsigned i = 0; i < IMM_NUCLT_SIZE; ++i)
    {
        if (!io_write_imm_float(&ctx, nucltp->lprobs[i]))
            return error(IMM_IOERROR, "failed to write imm_float");
    }
    return IMM_SUCCESS;
}

enum imm_rc imm_nuclt_lprob_read(struct imm_nuclt_lprob *nucltp, FILE *file)
{
    cmp_ctx_t cmp = {0};
    xcmp_fsetup(&cmp, file);
    return imm_nuclt_lprob_read_cmp(nucltp, &cmp);
}

enum imm_rc imm_nuclt_lprob_read_cmp(struct imm_nuclt_lprob *nucltp,
                                     struct cmp_ctx_s *cmp)
{
    uint32_t size = 0;
    cmp_read_array(cmp, &size);
    assert(size == IMM_NUCLT_SIZE);
    for (unsigned i = 0; i < IMM_NUCLT_SIZE; ++i)
    {
        if (!io_read_imm_float(cmp, nucltp->lprobs + i))
            return error(IMM_IOERROR, "failed to read imm_float");
    }
    return IMM_SUCCESS;
}

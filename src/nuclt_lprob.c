#include "imm/nuclt_lprob.h"
#include "error.h"
#include "imm/rc.h"
#include "xcw.h"

enum imm_rc imm_nuclt_lprob_pack(struct imm_nuclt_lprob const *nucltp,
                                 struct cw_pack_context *ctx)
{
    cw_pack_array_size(ctx, IMM_NUCLT_SIZE);
    for (unsigned i = 0; i < IMM_NUCLT_SIZE; ++i)
        cw_pack_imm_float(ctx, nucltp->lprobs[i]);

    return ctx->return_code ? error(IMM_IOERROR, "failed to pack nuclt_lprob")
                            : IMM_SUCCESS;
}

enum imm_rc imm_nuclt_lprob_unpack(struct imm_nuclt_lprob *nucltp,
                                   struct cw_unpack_context *ctx)
{
    if (cw_unpack_next_array_size(ctx) != IMM_NUCLT_SIZE)
        return error(IMM_IOERROR, "wrong array size");

    for (unsigned i = 0; i < IMM_NUCLT_SIZE; ++i)
        nucltp->lprobs[i] = cw_unpack_next_imm_float(ctx);

    return ctx->return_code ? error(IMM_IOERROR, "failed to unpack nuclt_lprob")
                            : IMM_SUCCESS;
}

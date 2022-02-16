#include "imm/nuclt_lprob.h"
#include "error.h"
#include "expect.h"
#include "imm/rc.h"
#include "lite_pack.h"

enum imm_rc imm_nuclt_lprob_pack(struct imm_nuclt_lprob const *nucltp,
                                 struct lip_io_file *io)
{
    lip_write_1darray_float(io, IMM_NUCLT_SIZE, nucltp->lprobs);

    return io->error ? error(IMM_IOERROR, "failed to pack nuclt_lprob")
                     : IMM_SUCCESS;
}

enum imm_rc imm_nuclt_lprob_unpack(struct imm_nuclt_lprob *nucltp,
                                   struct lip_io_file *io)
{
    expect_1darray_float_type(io, IMM_NUCLT_SIZE, XLIP_1DARRAY_FLOAT,
                              nucltp->lprobs);

    return io->error ? error(IMM_IOERROR, "failed to unpack nuclt_lprob")
                     : IMM_SUCCESS;
}

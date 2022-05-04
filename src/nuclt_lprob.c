#include "imm/nuclt_lprob.h"
#include "error.h"
#include "expect.h"
#include "imm/rc.h"
#include "lite_pack.h"

enum imm_rc imm_nuclt_lprob_pack(struct imm_nuclt_lprob const *nucltp,
                                 struct lip_file *file)
{
    lip_write_1darray_float(file, IMM_NUCLT_SIZE, nucltp->lprobs);

    return file->error ? error(IMM_NUCLT_LPROB_IO_FAIL) : IMM_OK;
}

enum imm_rc imm_nuclt_lprob_unpack(struct imm_nuclt_lprob *nucltp,
                                   struct lip_file *file)
{
    expect_1darray_float_type(file, IMM_NUCLT_SIZE, nucltp->lprobs);

    return file->error ? error(IMM_NUCLT_LPROB_IO_FAIL) : IMM_OK;
}

#include "imm/nuclt_lprob.h"
#include "error.h"
#include "expect.h"
#include "imm/rc.h"
#include "lip.h"

int imm_nuclt_lprob_pack(struct imm_nuclt_lprob const *nucltp,
                         struct lip_file *file)
{
    lip_write_1darray_float(file, IMM_NUCLT_SIZE, nucltp->lprobs);

    return file->error ? IMM_NUCLT_LPROB_IO_FAIL : IMM_OK;
}

int imm_nuclt_lprob_unpack(struct imm_nuclt_lprob *nucltp,
                           struct lip_file *file)
{
    imm_expect_1darr_float_type(file, IMM_NUCLT_SIZE, nucltp->lprobs);

    return file->error ? IMM_NUCLT_LPROB_IO_FAIL : IMM_OK;
}

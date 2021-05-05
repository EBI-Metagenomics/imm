#include "imm/seq.h"
#include "common/common.h"
#include "imm/abc.h"

struct imm_seq const imm_seq_empty = {
    .len = 0,
    .str = "",
    .abc = &imm_abc_empty,
};

int imm_seq_init(struct imm_seq *seq, struct imm_str str,
                 struct imm_abc const *abc)
{
    for (unsigned i = 0; i < str.len; ++i)
    {
        if (!imm_abc_has_symbol(abc, str.data[i]) &&
            str.data[i] != imm_abc_any_symbol(abc))
        {
            return xerror(IMM_ILLEGALARG, "symbol not found in the alphabet");
        }
    }
    seq->len = str.len;
    seq->str = str.data;
    seq->abc = abc;
    return IMM_SUCCESS;
}

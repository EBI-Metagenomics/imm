#include "imm/seq.h"
#include "common/common.h"
#include "imm/abc.h"
#include "imm/error.h"

void imm_seq_del(struct imm_seq const *seq) { free((void *)seq); }

struct imm_seq const *imm_seq_new(struct imm_str str, struct imm_abc const *abc)
{
    for (unsigned i = 0; i < str.len; ++i)
    {
        if (!imm_abc_has_symbol(abc, str.data[i]) &&
            str.data[i] != imm_abc_any_symbol(abc))
        {
            xerror(IMM_ILLEGALARG, "symbol not found in the alphabet");
            return NULL;
        }
    }
    struct imm_seq *seq = xmalloc(sizeof(*seq));
    seq->len = str.len;
    seq->str = str.data;
    seq->abc = abc;
    return seq;
}

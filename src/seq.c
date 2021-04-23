#include "imm/seq.h"
#include "common/common.h"
#include "imm/abc.h"

struct imm_seq const *imm_seq_new(uint32_t len, char const *str,
                                  struct imm_abc const *abc)
{
    for (unsigned i = 0; i < len; ++i)
    {
        if (!imm_abc_has_symbol(abc, str[i]) &&
            str[i] != imm_abc_any_symbol(abc))
        {
            error("symbol not found in the alphabet");
            return NULL;
        }
    }

    struct imm_seq *seq = xmalloc(sizeof(*seq));
    seq->len = len;
    seq->str = str;
    seq->abc = abc;
    return seq;
}

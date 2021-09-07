#include "imm/seq.h"
#include "error.h"
#include "imm/abc.h"
#include <assert.h>

struct imm_seq const imm_seq_empty = {
    .size = 0,
    .str = "",
    .abc = &imm_abc_empty,
};

#define UNUSED(x) (void)(x)

struct imm_seq imm_seq(struct imm_str str, struct imm_abc const *abc)
{
    for (unsigned i = 0; i < str.size; ++i)
    {
        char const any = imm_abc_any_symbol(abc);
        assert(imm_abc_has_symbol(abc, str.data[i]) || str.data[i] == any);
        UNUSED(any);
    }

    return (struct imm_seq){str.size, str.data, abc};
}

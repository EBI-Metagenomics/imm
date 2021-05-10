#include "imm/seq.h"
#include "common/common.h"
#include "imm/abc.h"

struct imm_seq const imm_seq_empty = {
    .len = 0,
    .str = "",
    .abc = &imm_abc_empty,
};

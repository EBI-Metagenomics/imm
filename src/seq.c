#include "imm/seq.h"
#include "imm/abc.h"
#include "support.h"

struct imm_seq const imm_seq_empty = {
    .size = 0,
    .str = "",
    .abc = &imm_abc_empty,
};

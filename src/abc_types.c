#include "imm/abc_types.h"

static char const abc_typeid_name[][9] = {"null_abc", "abc", "amino",
                                          "nuclt",    "dna", "rna"};

char const *imm_abc_typeid_name(enum imm_abc_typeid typeid)
{
    return abc_typeid_name[typeid];
}

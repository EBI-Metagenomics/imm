#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "src/imm/hide.h"

struct imm_abc;

HIDE int abc_has_symbols(struct imm_abc const* abc, char const* seq, int seq_len);

#endif

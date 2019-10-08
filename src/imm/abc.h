#ifndef IMM_ABC_H
#define IMM_ABC_H

#include "imm.h"
#include "src/imm/hide.h"

HIDE int abc_length(const struct imm_abc *abc);
HIDE int abc_has_symbol(const struct imm_abc *abc, char symbol_id);
HIDE int abc_symbol_idx(const struct imm_abc *abc, char symbol_id);
HIDE char abc_symbol_id(const struct imm_abc *abc, int symbol_idx);

#endif

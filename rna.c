#include "rna.h"

int imm_rna_init(struct imm_rna *rna, char const *symbols, char any_symbol)
{
  return imm__abc_init(&rna->super.super, IMM_RNA_SIZE, symbols, any_symbol,
                       IMM_RNA);
}

#define ID(c) IMM_SYM_ID(c)
#define SET(c, idx) [ID(c)] = idx
#define NIL(x, y) [ID(x)... ID(y) - 1] = IMM_SYM_NULL_IDX

struct imm_rna const imm_rna_iupac = {
    .super.super = {
        .typeid = IMM_RNA,
        .size = IMM_RNA_SIZE,
        .symbols = IMM_RNA_SYMBOLS,
        .sym = {.idx =
                    {
                        NIL(IMM_SYM_FIRST_CHAR, 'A'),
                        SET('A', 0),
                        IMM_SYM_NULL_IDX,
                        SET('C', 1),
                        NIL('D', 'G'),
                        SET('G', 2),
                        NIL('H', 'U'),
                        SET('U', 3),
                        NIL('V', IMM_RNA_ANY_SYMBOL),
                        SET(IMM_RNA_ANY_SYMBOL, IMM_RNA_SIZE),
                        NIL('Y', IMM_SYM_LAST_CHAR),
                    }},
        .any_symbol_id = IMM_SYM_ID(IMM_RNA_ANY_SYMBOL),
    }};

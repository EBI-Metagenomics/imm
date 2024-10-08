#include "imm_dna.h"

int imm_dna_init(struct imm_dna *dna, char const *symbols, char any_symbol)
{
  return imm__abc_init(&dna->super.super, IMM_DNA_SIZE, symbols, any_symbol,
                       IMM_DNA);
}

#define ID(c) IMM_SYM_ID(c)
#define SET(c, idx) [ID(c)] = idx
#define NIL(x, y) [ID(x)... ID(y) - 1] = IMM_SYM_NULL_IDX

struct imm_dna const imm_dna_iupac = {
    .super.super = {
        .typeid = IMM_DNA,
        .size = IMM_DNA_SIZE,
        .symbols = IMM_DNA_SYMBOLS,
        .sym = {.idx =
                    {
                        NIL(IMM_SYM_FIRST_CHAR, 'A'),
                        SET('A', 0),
                        IMM_SYM_NULL_IDX,
                        SET('C', 1),
                        NIL('D', 'G'),
                        SET('G', 2),
                        NIL('H', 'T'),
                        SET('T', 3),
                        NIL('U', IMM_DNA_ANY_SYMBOL),
                        SET(IMM_DNA_ANY_SYMBOL, IMM_DNA_SIZE),
                        NIL('Y', IMM_SYM_LAST_CHAR),
                    }},
        .any_symbol_id = IMM_SYM_ID(IMM_DNA_ANY_SYMBOL),
    }};

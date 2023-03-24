#include "imm/nuclt.h"
#include "abc.h"

struct imm_nuclt const imm_nuclt_empty = {ABC_EMPTY};

int imm_nuclt_init(struct imm_nuclt *nuclt, char const symbols[IMM_NUCLT_SIZE],
                   char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_NUCLT, nuclt};
    return imm__abc_init(&nuclt->super, IMM_NUCLT_SIZE, symbols, any_symbol,
                         vtable);
}

#define ID(c) IMM_SYM_ID(c)
#define SET(c, idx) [ID(c)] = idx
#define NIL(x, y) [ID(x)... ID(y) - 1] = IMM_SYM_NULL_IDX

struct imm_nuclt const imm_nuclt_default = {
    .super = {
        .size = IMM_NUCLT_SIZE,
        .symbols = IMM_NUCLT_SYMBOLS,
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
                        NIL('U', IMM_NUCLT_ANY_SYMBOL),
                        SET(IMM_NUCLT_ANY_SYMBOL, IMM_NUCLT_SIZE),
                        NIL('Y', IMM_SYM_LAST_CHAR),
                    }},
        .any_symbol_id = IMM_SYM_ID(IMM_NUCLT_ANY_SYMBOL),
        .vtable = {IMM_NUCLT, NULL},
    }};

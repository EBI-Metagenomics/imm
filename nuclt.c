#include "nuclt.h"

int imm_nuclt_init(struct imm_nuclt *x, char const *symbols, char any_symbol)
{
  return imm__abc_init(&x->super, IMM_NUCLT_SIZE, symbols, any_symbol,
                       IMM_NUCLT);
}

int imm_nuclt_size(struct imm_nuclt const *x)
{
  (void)x;
  return IMM_NUCLT_SIZE;
}

#define ID(c) IMM_SYM_ID(c)
#define SET(c, idx) [ID(c)] = idx
#define NIL(x, y) [ID(x)... ID(y) - 1] = IMM_SYM_NULL_IDX

struct imm_nuclt const imm_nuclt_default = {
    .super = {
        .typeid = IMM_NUCLT,
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
    }};

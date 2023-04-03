#include "amino.h"
#include "abc.h"

int imm_amino_init(struct imm_amino *amino, char const *symbols,
                   char any_symbol)
{
  return imm__abc_init(&amino->super, IMM_AMINO_SIZE, symbols, any_symbol,
                       IMM_AMINO);
}

#define ID(c) IMM_SYM_ID(c)
#define SET(c, idx) [ID(c)] = idx
#define NIL(x, y) [ID(x)... ID(y) - 1] = IMM_SYM_NULL_IDX

struct imm_amino const
    imm_amino_iupac =
        {.super =
             {
                 .typeid = IMM_AMINO,
                 .size = IMM_AMINO_SIZE,
                 .symbols = IMM_AMINO_SYMBOLS,
                 .sym = {.idx =
                             {
                                 NIL(IMM_SYM_FIRST_CHAR, 'A'),
                                 SET('A', 0),
                                 NIL('B', 'C'),
                                 SET('C', 1),
                                 SET('D', 2),
                                 SET('E', 3),
                                 SET('F', 4),
                                 SET('G', 5),
                                 SET('H', 6),
                                 SET('I', 7),
                                 NIL('J', 'K'),
                                 SET('K', 8),
                                 SET('L', 9),
                                 SET('M', 10),
                                 SET('N', 11),
                                 NIL('O', 'P'),
                                 SET('P', 12),
                                 SET('Q', 13),
                                 SET('R', 14),
                                 SET('S', 15),
                                 SET('T', 16),
                                 NIL('U', 'V'),
                                 SET('V', 17),
                                 SET('W', 18),
                                 SET(IMM_AMINO_ANY_SYMBOL, IMM_AMINO_SIZE),
                                 SET('Y', 19),
                                 NIL('Z', IMM_SYM_LAST_CHAR),
                             }},
                 .any_symbol_id = IMM_SYM_ID(IMM_AMINO_ANY_SYMBOL),
             }};

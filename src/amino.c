#include "imm/amino.h"
#include "abc.h"
#include "imm/abc.h"

int imm_amino_init(struct imm_amino *amino, char const symbols[IMM_AMINO_SIZE],
                   char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_AMINO, amino};
    return abc_init(&amino->super, IMM_AMINO_SIZE, symbols, any_symbol, vtable);
}

int imm_amino_write(struct imm_amino const *amino, FILE *file)
{
    return imm_abc_write(&amino->super, file);
}

int imm_amino_read(struct imm_amino *amino, FILE *file)
{
    return imm_abc_read(&amino->super, file);
}

#define ID(c) IMM_SYM_ID(c)
#define SET(c, idx) [ID(c)] = idx
#define NIL(x, y) [ID(x)... ID(y) - 1] = IMM_SYM_NULL_IDX

struct imm_amino const
    imm_amino_default =
        {.super =
             {
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
                 .vtable = {IMM_AMINO, NULL},
             }};

#include "amino.h"
#include "abc.h"
#include "common/common.h"
#include "imm/amino.h"

static char const amino_symbols[IMM_AMINO_NSYMBOLS + 1] = IMM_AMINO_SYMBOLS;

struct imm_amino imm_amino_default = {
    .super = {
        .nsymbols = IMM_AMINO_NSYMBOLS,
        .symbols = amino_symbols,
        .symbol_idx =
            {
                [0 ... __IMM_SYM_INDEX('A') - 1] = IMM_SYM_NULL_IDX,
                [__IMM_SYM_INDEX('A')] = 0,
                IMM_SYM_NULL_IDX,
                [__IMM_SYM_INDEX('C')] = 1,
                [__IMM_SYM_INDEX('D')] = 2,
                [__IMM_SYM_INDEX('E')] = 3,
                [__IMM_SYM_INDEX('F')] = 4,
                [__IMM_SYM_INDEX('G')] = 5,
                [__IMM_SYM_INDEX('H')] = 6,
                [__IMM_SYM_INDEX('I')] = 7,
                IMM_SYM_NULL_IDX,
                [__IMM_SYM_INDEX('K')] = 8,
                [__IMM_SYM_INDEX('L')] = 9,
                [__IMM_SYM_INDEX('M')] = 10,
                [__IMM_SYM_INDEX('N')] = 11,
                IMM_SYM_NULL_IDX,
                [__IMM_SYM_INDEX('P')] = 12,
                [__IMM_SYM_INDEX('Q')] = 13,
                [__IMM_SYM_INDEX('R')] = 14,
                [__IMM_SYM_INDEX('S')] = 15,
                [__IMM_SYM_INDEX('T')] = 16,
                IMM_SYM_NULL_IDX,
                [__IMM_SYM_INDEX('V')] = 17,
                [__IMM_SYM_INDEX('W')] = 18,
                IMM_SYM_NULL_IDX,
                [__IMM_SYM_INDEX('Y')] = 19,
                [__IMM_SYM_INDEX('Y') + 1 ... __IMM_SYM_IDX_SIZE - 1] =
                    IMM_SYM_NULL_IDX,
            },
        .any_symbol = IMM_AMINO_ANY_SYMBOL,
        .vtable = {IMM_AMINO, NULL},
    }};

int imm_amino_init(struct imm_amino *amino)
{
    struct imm_abc_vtable vtable = {IMM_AMINO, amino};
    return abc_init(&amino->super, IMM_AMINO_NSYMBOLS, amino_symbols,
                    IMM_AMINO_ANY_SYMBOL, vtable);
}

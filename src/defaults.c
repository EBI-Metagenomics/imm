#include "abc.h"
#include "common/common.h"
#include "imm/amino.h"
#include "imm/dna.h"
#include "imm/nuclt.h"
#include "imm/rna.h"

#define IDX(c) __IMM_SYM_INDEX(c)
#define SET(c, idx) [IDX(c)] = idx
#define NIL(x, y) [IDX(x)... IDX(y) - 1] = IMM_SYM_NULL_IDX

struct imm_amino const imm_amino_default = {
    .super = {
        .nsymbols = IMM_AMINO_NSYMBOLS,
        .symbols = IMM_AMINO_SYMBOLS,
        .symbol_idx =
            {
                NIL(__IMM_SYM_FIRST, 'A'),
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
                NIL('X', 'Y'),
                SET('Y', 19),
                NIL('Z', __IMM_SYM_LAST),
            },
        .any_symbol = IMM_AMINO_ANY_SYMBOL,
        .vtable = {IMM_AMINO, NULL},
    }};

struct imm_nuclt const imm_nuclt_default = {
    .super = {
        .nsymbols = IMM_NUCLT_NSYMBOLS,
        .symbols = IMM_NUCLT_SYMBOLS,
        .symbol_idx =
            {
                NIL(__IMM_SYM_FIRST, 'A'),
                SET('A', 0),
                IMM_SYM_NULL_IDX,
                SET('C', 1),
                [IDX('D')... IDX('F')] = IMM_SYM_NULL_IDX,
                SET('G', 2),
                [IDX('H')... IDX('S')] = IMM_SYM_NULL_IDX,
                SET('T', 3),
                NIL('U', __IMM_SYM_LAST),
            },
        .any_symbol = IMM_NUCLT_ANY_SYMBOL,
        .vtable = {IMM_NUCLT, NULL},
    }};

struct imm_dna const imm_dna_default = {.super.super = {
                                            .nsymbols = IMM_DNA_NSYMBOLS,
                                            .symbols = IMM_DNA_SYMBOLS,
                                            .symbol_idx =
                                                {
                                                    NIL(__IMM_SYM_FIRST, 'A'),
                                                    SET('A', 0),
                                                    IMM_SYM_NULL_IDX,
                                                    SET('C', 1),
                                                    NIL('D', 'G'),
                                                    SET('G', 2),
                                                    NIL('H', 'T'),
                                                    SET('T', 3),
                                                    NIL('U', __IMM_SYM_LAST),
                                                },
                                            .any_symbol = IMM_DNA_ANY_SYMBOL,
                                            .vtable = {IMM_DNA, NULL},
                                        }};

struct imm_rna const imm_rna_default = {.super.super = {
                                            .nsymbols = IMM_RNA_NSYMBOLS,
                                            .symbols = IMM_RNA_SYMBOLS,
                                            .symbol_idx =
                                                {
                                                    NIL(__IMM_SYM_FIRST, 'A'),
                                                    SET('A', 0),
                                                    IMM_SYM_NULL_IDX,
                                                    SET('C', 1),
                                                    NIL('D', 'G'),
                                                    SET('G', 2),
                                                    NIL('H', 'U'),
                                                    SET('U', 3),
                                                    NIL('V', __IMM_SYM_LAST),
                                                },
                                            .any_symbol = IMM_RNA_ANY_SYMBOL,
                                            .vtable = {IMM_RNA, NULL},
                                        }};

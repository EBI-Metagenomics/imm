#include "abc.h"
#include "common/common.h"
#include "imm/amino.h"
#include "imm/dna.h"
#include "imm/nuclt.h"
#include "imm/rna.h"

#define ID(c) IMM_SYM_ID(c)
#define SET(c, idx) [ID(c)] = idx
#define NIL(x, y) [ID(x)... ID(y) - 1] = IMM_SYM_NULL_IDX

struct imm_amino const
    imm_amino_default =
        {.super =
             {
                 .nsymbols = IMM_AMINO_NSYMBOLS,
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
                                 SET(IMM_AMINO_ANY_SYMBOL, IMM_AMINO_NSYMBOLS),
                                 SET('Y', 19),
                                 NIL('Z', IMM_SYM_LAST_CHAR),
                             }},
                 .any_symbol_id = IMM_SYM_ID(IMM_AMINO_ANY_SYMBOL),
                 .vtable = {IMM_AMINO, NULL},
             }};

struct imm_nuclt const imm_nuclt_default = {
    .super = {
        .nsymbols = IMM_NUCLT_NSYMBOLS,
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
                        SET(IMM_NUCLT_ANY_SYMBOL, IMM_NUCLT_NSYMBOLS),
                        NIL('Y', IMM_SYM_LAST_CHAR),
                    }},
        .any_symbol_id = IMM_SYM_ID(IMM_NUCLT_ANY_SYMBOL),
        .vtable = {IMM_NUCLT, NULL},
    }};

struct imm_dna const imm_dna_default = {
    .super.super = {
        .nsymbols = IMM_DNA_NSYMBOLS,
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
                        SET(IMM_DNA_ANY_SYMBOL, IMM_DNA_NSYMBOLS),
                        NIL('Y', IMM_SYM_LAST_CHAR),
                    }},
        .any_symbol_id = IMM_SYM_ID(IMM_DNA_ANY_SYMBOL),
        .vtable = {IMM_DNA, NULL},
    }};

struct imm_rna const imm_rna_default = {
    .super.super = {
        .nsymbols = IMM_RNA_NSYMBOLS,
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
                        SET(IMM_RNA_ANY_SYMBOL, IMM_RNA_NSYMBOLS),
                        NIL('Y', IMM_SYM_LAST_CHAR),
                    }},
        .any_symbol_id = IMM_SYM_ID(IMM_RNA_ANY_SYMBOL),
        .vtable = {IMM_RNA, NULL},
    }};

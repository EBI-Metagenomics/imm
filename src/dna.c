#include "imm/dna.h"
#include "abc.h"

enum imm_rc imm_dna_init(struct imm_dna *dna, char const symbols[IMM_DNA_SIZE],
                         char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_DNA, dna};
    return abc_init(&dna->super.super, IMM_DNA_SIZE, symbols, any_symbol,
                    vtable);
}

enum imm_rc imm_dna_write(struct imm_dna const *dna, FILE *file)
{
    return imm_abc_write(&dna->super.super, file);
}

enum imm_rc imm_dna_read(struct imm_dna *dna, FILE *file)
{
    return imm_abc_read(&dna->super.super, file);
}

#define ID(c) IMM_SYM_ID(c)
#define SET(c, idx) [ID(c)] = idx
#define NIL(x, y) [ID(x)... ID(y) - 1] = IMM_SYM_NULL_IDX

struct imm_dna const imm_dna_default = {
    .super.super = {
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
        .vtable = {IMM_DNA, NULL},
    }};

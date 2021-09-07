#ifndef ABC_H
#define ABC_H

#include "imm/abc.h"

enum imm_rc abc_init(struct imm_abc *abc, unsigned len, char const *symbols,
                     char any_symbol, struct imm_abc_vtable vtable);
enum imm_rc abc_write(struct imm_abc const *abc, FILE *file);
enum imm_rc abc_read(struct imm_abc *abc, FILE *file);

#define ABC_EMPTY                                                              \
    {                                                                          \
        .size = 0, .symbols = {'\0'},                                          \
        .sym = {.idx[0 ...(IMM_SYM_SIZE) - 1] = IMM_SYM_NULL_IDX},             \
        .any_symbol_id = IMM_SYM_ID('X'), .vtable = {                          \
            IMM_ABC,                                                           \
            NULL                                                               \
        }                                                                      \
    }

#endif

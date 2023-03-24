#ifndef ABC_H
#define ABC_H

#include "imm/abc.h"

int imm__abc_init(struct imm_abc *abc, unsigned len, char const *symbols,
                  char any_symbol, struct imm_abc_vtable vtable);
int imm__abc_pack(struct imm_abc const *abc, struct lip_file *);
int imm__abc_unpack(struct imm_abc *abc, struct lip_file *);

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

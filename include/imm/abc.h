#ifndef IMM_ABC_H_API
#define IMM_ABC_H_API

#include "imm/api.h"

#define IMM_ANY_SYMBOL '*'
#define IMM_NULL_SYMBOL '\0'

struct imm_abc;

IMM_API struct imm_abc* imm_abc_create(char const* symbols);
IMM_API void            imm_abc_destroy(struct imm_abc* abc);
IMM_API int             imm_abc_length(struct imm_abc const* abc);
IMM_API int             imm_abc_has_symbol(struct imm_abc const* abc, char symbol_id);
IMM_API int             imm_abc_symbol_idx(struct imm_abc const* abc, char symbol_id);
IMM_API char            imm_abc_symbol_id(struct imm_abc const* abc, int symbol_idx);

#endif

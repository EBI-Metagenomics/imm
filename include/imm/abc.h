#ifndef IMM_ABC_H_API
#define IMM_ABC_H_API

#include "imm/api.h"

#define IMM_ANY_SYMBOL '*'

struct imm_abc;

IMM_API struct imm_abc *imm_abc_create(const char *symbols);
IMM_API void imm_abc_destroy(struct imm_abc *abc);

#endif

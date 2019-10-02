#ifndef IMM_ALPHABET_H_API
#define IMM_ALPHABET_H_API

#include "imm/api.h"
#include <stddef.h>

#define IMM_ANY_SYMBOL '*'

struct imm_alphabet;

IMM_API struct imm_alphabet *imm_alphabet_create(const char *symbols);
IMM_API void imm_alphabet_destroy(struct imm_alphabet *alphabet);

#endif

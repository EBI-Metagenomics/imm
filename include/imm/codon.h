#ifndef IMM_CODON_H_API
#define IMM_CODON_H_API

#include "imm/api.h"

struct imm_codon;

IMM_API struct imm_codon *imm_codon_create(void);
IMM_API struct imm_codon *imm_codon_clone(const struct imm_codon *codon);
IMM_API void imm_codon_set_lprob(struct imm_codon *codon, int a, int b, int c,
                                   double lprob);
IMM_API void imm_codon_set_ninfs(struct imm_codon *codon);
IMM_API double imm_codon_get_lprob(const struct imm_codon *codon, int a, int b,
                                     int c);
IMM_API int imm_codon_normalize(struct imm_codon *codon);
IMM_API void imm_codon_destroy(struct imm_codon *codon);

#endif

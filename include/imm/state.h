#ifndef IMM_STATE_H_API
#define IMM_STATE_H_API

#include "imm/api.h"
#include <stdbool.h>
#include <stddef.h>

#define IMM_INVALID_STATE_ID -1

struct imm_alphabet;
struct imm_codon;
struct imm_state;

IMM_API struct imm_state *imm_state_create_normal(const char *name,
                                                  const struct imm_alphabet *alphabet,
                                                  const double *emiss_lprobs);

IMM_API struct imm_state *imm_state_create_silent(const char *name,
                                                  const struct imm_alphabet *alphabet);

IMM_API struct imm_state *imm_state_create_frame(const char *name,
                                                 const struct imm_alphabet *alphabet,
                                                 const double *base_lprobs,
                                                 const struct imm_codon *codon,
                                                 double epsilon);

IMM_API const char *imm_state_get_name(const struct imm_state *state);

IMM_API const struct imm_alphabet *imm_state_get_alphabet(
    const struct imm_state *state);

IMM_API void imm_state_set_end_state(struct imm_state *state, bool end_state);

IMM_API double imm_state_emiss_lprob(const struct imm_state *state, const char *seq,
                                     size_t seq_len);

IMM_API int imm_state_normalize(struct imm_state *state);

IMM_API size_t imm_state_min_seq(const struct imm_state *state);
IMM_API size_t imm_state_max_seq(const struct imm_state *state);

IMM_API void imm_state_destroy(struct imm_state *state);

#endif

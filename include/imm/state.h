#ifndef IMM_STATE_H_API
#define IMM_STATE_H_API

#include "imm/api.h"

struct imm_abc;
struct imm_state;

typedef double (*imm_state_lprob_t)(const struct imm_state *state, const char *seq,
                                    int seq_len);
typedef int (*imm_state_min_seq_t)(const struct imm_state *state);
typedef int (*imm_state_max_seq_t)(const struct imm_state *state);

struct imm_state_funcs
{
    imm_state_lprob_t lprob;
    imm_state_min_seq_t min_seq;
    imm_state_max_seq_t max_seq;
};

IMM_API struct imm_state *imm_state_create(const char *name, const struct imm_abc *abc,
                                           struct imm_state_funcs funcs, void *impl);
IMM_API void imm_state_destroy(struct imm_state *state);

IMM_API void *imm_state_get_impl(struct imm_state *state);
IMM_API const void *imm_state_get_impl_c(const struct imm_state *state);

IMM_API const char *imm_state_get_name(const struct imm_state *state);

IMM_API const struct imm_abc *imm_state_get_abc(const struct imm_state *state);

IMM_API double imm_state_lprob(const struct imm_state *state, const char *seq, int seq_len);
IMM_API int imm_state_min_seq(const struct imm_state *state);
IMM_API int imm_state_max_seq(const struct imm_state *state);

IMM_API struct imm_state *imm_state_cast(void *state);
IMM_API const struct imm_state *imm_state_cast_c(const void *state);

#endif

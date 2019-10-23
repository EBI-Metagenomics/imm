#ifndef IMM_HMM_H_API
#define IMM_HMM_H_API

#include "imm/api.h"

struct imm_abc;
struct imm_hmm;
struct imm_path;
struct imm_state;

IMM_API struct imm_hmm *imm_hmm_create(const struct imm_abc *abc);
IMM_API void imm_hmm_destroy(struct imm_hmm *hmm);

IMM_API int imm_hmm_add_state(struct imm_hmm *hmm, const struct imm_state *state,
                              double start_lprob);
IMM_API int imm_hmm_del_state(struct imm_hmm *hmm, const struct imm_state *state);

IMM_API int imm_hmm_set_start_lprob(struct imm_hmm *hmm, const struct imm_state *state,
                                    double start_lprob);

IMM_API int imm_hmm_set_trans(struct imm_hmm *hmm, const struct imm_state *src_state,
                              const struct imm_state *dst_state, double lprob);
IMM_API double imm_hmm_get_trans(const struct imm_hmm *hmm, const struct imm_state *src_state,
                                 const struct imm_state *dst_state);

IMM_API double imm_hmm_likelihood(const struct imm_hmm *hmm, const char *seq,
                                  const struct imm_path *path);

IMM_API double imm_hmm_viterbi(const struct imm_hmm *hmm, const char *seq,
                               const struct imm_state *end_state);

IMM_API int imm_hmm_normalize(struct imm_hmm *hmm);

IMM_API int imm_hmm_normalize_start(struct imm_hmm *hmm);

IMM_API int imm_hmm_normalize_trans(struct imm_hmm *hmm, const struct imm_state *src);

#endif

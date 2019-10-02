#ifndef IMM_HMM_H_API
#define IMM_HMM_H_API

#include "imm/api.h"

struct imm_alphabet;
struct imm_hmm;
struct imm_path;
struct imm_state;

IMM_API struct imm_hmm *imm_hmm_create(const struct imm_alphabet *alphabet);
IMM_API int imm_hmm_add_state(struct imm_hmm *hmm, const struct imm_state *state,
                              double start_lprob);
IMM_API int imm_hmm_del_state(struct imm_hmm *hmm, int state_id);
IMM_API const struct imm_state *imm_hmm_get_state(const struct imm_hmm *hmm,
                                                  int state_id);
IMM_API int imm_hmm_set_trans(struct imm_hmm *hmm, int src_state_id, int dst_state_id,
                              double lprob);
IMM_API double imm_hmm_get_trans(const struct imm_hmm *hmm, int src_state_id,
                                 int dst_state_id);
IMM_API double imm_hmm_likelihood(const struct imm_hmm *hmm, const char *seq,
                                  const struct imm_path *path);
IMM_API int imm_hmm_normalize(struct imm_hmm *hmm);
IMM_API void imm_hmm_destroy(struct imm_hmm *hmm);

#endif

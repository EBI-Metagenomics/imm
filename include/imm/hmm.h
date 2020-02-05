#ifndef IMM_HMM_H
#define IMM_HMM_H

#include "imm/api.h"
#include "imm/seq.h"

struct imm_abc;
struct imm_hmm;
struct imm_path;
struct imm_state;

IMM_API struct imm_hmm* imm_hmm_create(struct imm_abc const* abc);
IMM_API void            imm_hmm_destroy(struct imm_hmm* hmm);

IMM_API int imm_hmm_add_state(struct imm_hmm* hmm, struct imm_state const* state,
                              double start_lprob);
IMM_API int imm_hmm_del_state(struct imm_hmm* hmm, struct imm_state const* state);

IMM_API int imm_hmm_set_start(struct imm_hmm* hmm, struct imm_state const* state,
                              double lprob);

IMM_API int    imm_hmm_set_trans(struct imm_hmm* hmm, struct imm_state const* src_state,
                                 struct imm_state const* dst_state, double lprob);
IMM_API double imm_hmm_get_trans(struct imm_hmm const* hmm, struct imm_state const* src_state,
                                 struct imm_state const* dst_state);

IMM_API double imm_hmm_likelihood(struct imm_hmm const* hmm, struct imm_seq const* seq,
                                  struct imm_path const* path);

IMM_API struct imm_results const* imm_hmm_viterbi(struct imm_hmm const*   hmm,
                                                  struct imm_seq const*   seq,
                                                  struct imm_state const* end_state,
                                                  unsigned                window_length);

IMM_API int imm_hmm_normalize(struct imm_hmm* hmm);

IMM_API int imm_hmm_normalize_start(struct imm_hmm* hmm);

IMM_API int imm_hmm_normalize_trans(struct imm_hmm* hmm, struct imm_state const* src);

#endif

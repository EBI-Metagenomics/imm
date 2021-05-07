#ifndef IMM_FRAME_STATE_H
#define IMM_FRAME_STATE_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/state_types.h"

/* struct imm_seq; */
struct imm_state;
struct imm_nuclt_lprob;
/* struct imm_codon; */
struct imm_codon_marg;
/* struct imm_frame_state; */
/* struct imm_profile; */

struct imm_frame_state
{
    struct imm_state *super;
    struct imm_nuclt_lprob const *nucltp;
    struct imm_codon_marg const *codonm;
    imm_float epsilon;
    imm_float leps;
    imm_float l1eps;
};

IMM_API struct imm_frame_state *
imm_frame_state_new(unsigned id, struct imm_nuclt_lprob const *nucltp,
                    struct imm_codon_marg const *codonm, imm_float epsilon);

#if 0
IMM_API struct imm_nuclt_lprob const *
imm_frame_state_base_lprob(struct imm_frame_state const *state);
IMM_API struct imm_codon_marg const *
imm_frame_state_codon_marg(struct imm_frame_state const *state);
IMM_API struct imm_frame_state const *
imm_frame_state_create(char const *name, struct imm_nuclt_lprob const *nucltp,
                       struct imm_codon_marg const *codonm, imm_float epsilon);
IMM_API imm_float imm_frame_state_decode(struct imm_frame_state const *state,
                                         struct imm_seq const *seq,
                                         struct imm_codon *codon);
IMM_API struct imm_frame_state const *
imm_frame_state_derived(struct imm_state const *state);
IMM_API void imm_frame_state_destroy(struct imm_frame_state const *state);
IMM_API imm_float imm_frame_state_epsilon(struct imm_frame_state const *state);
IMM_API imm_float imm_frame_state_lposterior(
    struct imm_frame_state const *state, struct imm_codon const *codon,
    struct imm_seq const *seq);
IMM_API struct imm_state const *
imm_frame_state_super(struct imm_frame_state const *state);

IMM_API struct imm_normal_state *
imm_normal_state_new(unsigned id, struct imm_nuclt const *nuclt,
                     imm_float const lprobs[1]);

IMM_API void imm_normal_state_del(struct imm_normal_state const *normal);

static inline struct imm_state *
imm_normal_state_super(struct imm_normal_state *normal)
{
    return normal->super;
}
#endif

#endif

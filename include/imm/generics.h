#ifndef IMM_GENERICS_H
#define IMM_GENERICS_H

#include "imm/amino.h"
#include "imm/hmm.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"
#include "imm/path.h"

#define imm_super(x)                                                           \
    _Generic((x), struct imm_normal_state *                                    \
             : imm_normal_state_super, struct imm_mute_state *                 \
             : imm_mute_state_super, struct imm_amino const *                  \
             : imm_amino_super)(x)

#define imm_del(x)                                                             \
    _Generic((x), struct imm_normal_state *                                    \
             : imm_normal_state_del, struct imm_mute_state *                   \
             : imm_mute_state_del,                                             \
             struct imm_seq const * : imm_seq_del,                             \
             struct imm_abc const * : imm_abc_del,                             \
             struct imm_task * : imm_task_del,                             \
             struct imm_hmm * : imm_hmm_del,                                   \
             struct imm_abc_lprob const * : imm_abc_lprob_del,                 \
             struct imm_amino const * : imm_amino_del) (x)

#define imm_deinit(x) _Generic((x), struct imm_path * : imm_path_deinit)(x)

#define imm_id(x) _Generic((x), struct imm_state * : imm_state_id)(x)

#endif
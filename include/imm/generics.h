#ifndef IMM_GENERICS_H
#define IMM_GENERICS_H

#include "imm/amino.h"
#include "imm/codon_state.h"
#include "imm/dna.h"
#include "imm/frame_state.h"
#include "imm/hmm.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"
#include "imm/path.h"
#include "imm/rna.h"

#define imm_super(x)                                                           \
    _Generic((x),                                                              \
             struct imm_normal_state * : imm_normal_state_super,               \
             struct imm_mute_state * : imm_mute_state_super,                   \
             struct imm_frame_state * : imm_frame_state_super,                 \
             struct imm_codon_state * : imm_codon_state_super,                 \
             struct imm_amino * : imm_amino_super,                             \
             struct imm_amino const * : imm_amino_super,                       \
             struct imm_dna * : imm_dna_super,                                 \
             struct imm_dna const * : imm_dna_super,                           \
             struct imm_rna * : imm_rna_super,                                 \
             struct imm_rna const * : imm_rna_super,                           \
             struct imm_nuclt * : imm_nuclt_super,                             \
             struct imm_nuclt const * : imm_nuclt_super)(x)

#define imm_len(x)                                                             \
    _Generic((x), struct imm_abc const * : imm_abc_len,                        \
            struct imm_nuclt const * : imm_nuclt_len)(x)

#define imm_del(x)                                                             \
    _Generic((x),                                                              \
             struct imm_normal_state *: imm_normal_state_del,                  \
             struct imm_mute_state *: imm_mute_state_del,                      \
             struct imm_codon_state *: imm_codon_state_del,                    \
             struct imm_frame_state *: imm_frame_state_del,                    \
             struct imm_abc const * : imm_abc_del,                             \
             struct imm_task * : imm_task_del,                                 \
             struct imm_hmm * : imm_hmm_del,                                   \
             struct imm_dp * : imm_dp_del) (x)

#define imm_deinit(x)                                                          \
    _Generic((x), struct imm_path *                                            \
             : imm_path_deinit, struct imm_result *                            \
             : imm_result_deinit)(x)

#define imm_id(x) _Generic((x), struct imm_state * : imm_state_id)(x)

#endif

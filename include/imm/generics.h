#ifndef IMM_GENERICS_H
#define IMM_GENERICS_H

#include "imm/mute_state.h"
#include "imm/normal_state.h"

#define imm_super(x)                                                           \
    _Generic((x), struct imm_normal_state *                                    \
             : imm_normal_state_super, struct imm_mute_state *                 \
             : imm_mute_state_super)(x)

#define imm_del(x)                                                             \
    _Generic((x), struct imm_normal_state *                                    \
             : imm_normal_state_del, struct imm_mute_state *               \
             : imm_mute_state_del, struct imm_seq const*                        \
             : imm_seq_del, struct imm_abc const*                               \
             : imm_abc_del)(x)

#endif

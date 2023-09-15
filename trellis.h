#ifndef IMM_TRELLIS_H
#define IMM_TRELLIS_H

#include "api.h"
#include "lprob.h"
#include "node.h"
#include "state.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Trellis representation for HMM paths
// ------------------------------------
//
// Consider the following HMM:
//
//                 ┌───┐        ┌───┐
//                 ▼   │        ▼   │
//   ┌──────┐     ┌────┴─┐     ┌────┴─┐
//   │  S0  ├────►│  S1  ├────►│  S2  │
//   └──────┘     └──────┘     └──────┘
//     Mute        Normal       Frame
//
// For which Mute emits no letter, Normal state emits a single letter, and
// Frame state emits from one or two letters.
//
// A trellis depiction for such states and a sequence of ACG is as follows:
//
//            A         C         G
//
// S0    @         @         @         @
//       │         │         │         │
//       │         │         │         │
//       ▼         ▼         ▼         ▼
// S1    @────────►@────────►@────────►@
//       │         └────┐    └─────────┐
//       └─────────┐    └────┐         │
//                 ▼         ▼         ▼
// S2    @────────►@────────►@────────►@
//       │         │         ▲         ▲
//       └─────────┼─────────┘         │
//                 └───────────────────┘
//
// The above diagram shows all possible state transitions that will match
// the provided sequence. The following shows a HMM solution path:
//
//            A         C         G
//
// S0    @         @         @         @
//       │
//       │
//       ▼
// S1    @         @         @         @
//       │
//       └─────────┐
//                 ▼
// S2    @         @         @         @
//                 │                   ▲
//                 └───────────────────┘
//
// Notice that the first transition captures no letter, the second on
// captures letter A, and the last one captures CG via the Frame state.
//
//
// A sequence of size (n-1) will have a trellis diagram with n stages
// (represented by n columns of @'s above). A HMM with m states will have
// a trellis diagram with m rows. A transition can be represented by a triplet:
//
//   (k,Si,h,Sj)
//
// for which h is the stage, Si and Sj are the source and destination states,
// and k is number of of captured letters. In particular, the path solution
// from the previous diagram can be described by the sequence
//
//   (0,S0,0,S1),(1,S1,1,S2),(2,S2,2,S3)
//
// A triplet (k,Si,h,Sj) is stored in this implementation by saving the source
// state Si and emission size k in `struct imm_node` at `imm_trellis_at(trellis,
// h, Sj)`.

struct imm_trellis
{
  uint16_t *ids;
  imm_state_name *state_name;
  size_t capacity;
  unsigned num_stages;
  unsigned num_states;
  struct imm_node *head;
  struct imm_node *pool;
};

IMM_API void imm_trellis_init(struct imm_trellis *);
IMM_API int imm_trellis_setup(struct imm_trellis *, unsigned seqsize,
                              unsigned nstates);
IMM_API void imm_trellis_cleanup(struct imm_trellis *);
IMM_API void imm_trellis_prepare(struct imm_trellis *);
IMM_API void imm_trellis_set_ids(struct imm_trellis *, uint16_t *ids);
IMM_API void imm_trellis_set_state_name(struct imm_trellis *, imm_state_name *);

IMM_API void imm_trellis_back(struct imm_trellis *);
IMM_API void imm_trellis_dump(struct imm_trellis const *, FILE *restrict);

IMM_INLINE void imm_trellis_rewind(struct imm_trellis *x) { x->head = x->pool; }

IMM_CONST size_t imm_trellis_size(struct imm_trellis const *x)
{
  return x->num_stages * x->num_states;
}

IMM_PURE struct imm_node *imm_trellis_head(struct imm_trellis const *x)
{
  return x->head;
}

IMM_PURE struct imm_node const *imm_trellis_at(struct imm_trellis const *x,
                                               unsigned stage, unsigned state)
{
  return x->pool + stage * x->num_states + state;
}

IMM_INLINE void imm_trellis_seek(struct imm_trellis *x, unsigned stage,
                                 unsigned state)
{
  x->head = (struct imm_node *)imm_trellis_at(x, stage, state);
  assert(x->head >= x->pool && x->head < x->pool + imm_trellis_size(x));
}

IMM_INLINE void imm_trellis_push(struct imm_trellis *x, float score,
                                 uint16_t state_source, uint8_t emissize)
{
  x->head->score = score;
  x->head->state_source = state_source;
  x->head->emission_size = emissize;
  x->head++;
}

IMM_PURE unsigned imm_trellis_state_idx_at(struct imm_trellis const *x,
                                           struct imm_node const *head)
{
  return (head - x->pool) % x->num_states;
}

IMM_PURE unsigned imm_trellis_state_idx(struct imm_trellis const *x)
{
  return imm_trellis_state_idx_at(x, x->head);
}

IMM_PURE unsigned imm_trellis_stage_idx(struct imm_trellis const *x)
{
  return (x->head - x->pool) / x->num_states;
}

#endif

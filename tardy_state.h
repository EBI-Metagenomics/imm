#ifndef IMM_TARDY_STATE_H
#define IMM_TARDY_STATE_H

// Tardy state is a mute state that has a transition to a state that comes
// before itself in the dynamic programming matrix. Those states must be
// evaluated before any other by the Viterbi algorithm.
struct tardy_state
{
  unsigned state_idx;
  unsigned trans_start;
};

#endif

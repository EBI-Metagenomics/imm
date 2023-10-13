#include "tsort.h"
#include "list.h"
#include "state.h"
#include "trans.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_MARK 0
#define TEMPORARY_MARK 1
#define PERMANENT_MARK 2

static bool check_mute_visit(struct imm_state **states, struct imm_state *state)
{
  if (imm_state_span(state).min > 0) return false;

  if (state->mark == PERMANENT_MARK) return false;

  if (state->mark == TEMPORARY_MARK) return true;

  state->mark = TEMPORARY_MARK;

  struct imm_trans *trans = NULL;
  imm_list_for_each_entry(trans, &state->trans.outgoing, outgoing)
  {
    if (check_mute_visit(states, states[trans->pair.idx.dst])) return true;
  }
  state->mark = PERMANENT_MARK;

  return false;
}

static bool check_mute_cycles(int nstates, struct imm_state **states)
{
  for (int i = 0; i < nstates; ++i)
  {
    if (check_mute_visit(states, states[i])) return false;
  }
  return true;
}

static void clear_marks(int nstates, struct imm_state **states)
{
  for (int i = 0; i < nstates; ++i)
    states[i]->mark = INITIAL_MARK;
}

static void visit(struct imm_state *state, struct imm_state **states, int *end,
                  struct imm_state **tmp)
{
  if (state->mark == PERMANENT_MARK) return;
  if (state->mark == TEMPORARY_MARK) return;

  state->mark = TEMPORARY_MARK;
  struct imm_trans const *trans = NULL;
  imm_list_for_each_entry(trans, &state->trans.outgoing, outgoing)
  {
    visit(states[trans->pair.idx.dst], states, end, tmp);
  }
  state->mark = PERMANENT_MARK;
  *end = *end - 1;
  tmp[*end] = state;
}

int imm_tsort(int nstates, struct imm_state **states, int start_idx)
{
  clear_marks(nstates, states);

  if (!check_mute_cycles(nstates, states)) return IMM_EMUTECYLES;

  clear_marks(nstates, states);

  struct imm_state **tmp = malloc(sizeof(struct imm_state *) * (size_t)nstates);
  if (!tmp) return IMM_ENOMEM;

  int end = nstates;
  visit(states[start_idx], states, &end, tmp);

  for (int i = 0; i < start_idx; ++i)
    visit(states[i], states, &end, tmp);

  for (int i = start_idx + 1; i < nstates; ++i)
    visit(states[i], states, &end, tmp);

  memcpy(states, tmp, sizeof(struct imm_state *) * (size_t)nstates);
  free(tmp);

  return 0;
}

#include "tsort.h"
#include "error.h"
#include "imm/state.h"
#include "imm/trans.h"
#include <stdlib.h>

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
    struct cco_iter it = cco_stack_iter(&state->trans.outgoing);
    cco_iter_for_each_entry(trans, &it, outgoing)
    {
        if (check_mute_visit(states, states[trans->pair.idx.dst])) return true;
    }
    state->mark = PERMANENT_MARK;

    return false;
}

static enum imm_rc check_mute_cycles(unsigned nstates,
                                     struct imm_state **states)
{
    for (unsigned i = 0; i < nstates; ++i)
    {
        if (check_mute_visit(states, states[i])) return IMM_FAILURE;
    }
    return IMM_SUCCESS;
}

static void clear_marks(unsigned nstates, struct imm_state **states)
{
    for (unsigned i = 0; i < nstates; ++i)
        states[i]->mark = INITIAL_MARK;
}

static void visit(struct imm_state *state, struct imm_state **states,
                  unsigned *end, struct imm_state **tmp)
{
    if (state->mark == PERMANENT_MARK) return;
    if (state->mark == TEMPORARY_MARK) return;

    state->mark = TEMPORARY_MARK;
    struct imm_trans const *trans = NULL;
    struct cco_iter it = cco_stack_iter(&state->trans.outgoing);
    cco_iter_for_each_entry(trans, &it, outgoing)
    {
        visit(states[trans->pair.idx.dst], states, end, tmp);
    }
    state->mark = PERMANENT_MARK;
    *end = *end - 1;
    tmp[*end] = state;
}

enum imm_rc tsort(unsigned nstates, struct imm_state **states,
                  unsigned start_idx)
{
    clear_marks(nstates, states);

    if (check_mute_cycles(nstates, states))
        return error(IMM_RUNTIMEERROR, "mute cycles are not allowed");

    clear_marks(nstates, states);

    struct imm_state **tmp = malloc(sizeof(*tmp) * nstates);
    if (!tmp) return error(IMM_OUTOFMEM, "failed to malloc");

    unsigned end = nstates;
    visit(states[start_idx], states, &end, tmp);

    for (unsigned i = 0; i < start_idx; ++i)
        visit(states[i], states, &end, tmp);

    for (unsigned i = start_idx + 1; i < nstates; ++i)
        visit(states[i], states, &end, tmp);

    memcpy(states, tmp, sizeof(*tmp) * nstates);
    free(tmp);

    return IMM_SUCCESS;
}

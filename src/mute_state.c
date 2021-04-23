#include "imm/mute_state.h"
#include "common/common.h"
#include "imm/float.h"
#include "imm/seq.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include <stdlib.h>

static void del(struct imm_state const *state);
static imm_float lprob(struct imm_state const *state,
                       struct imm_seq const *seq);
static uint8_t max_seq(struct imm_state const *state) { return 0; }
static uint8_t min_seq(struct imm_state const *state) { return 0; }
static uint8_t type_id(struct imm_state const *state)
{
    return IMM_MUTE_STATE_TYPE_ID;
}

static struct imm_state_vtable const vtable = {del, lprob, max_seq, min_seq,
                                               type_id};

struct imm_mute_state *imm_mute_state_new(uint16_t id,
                                          struct imm_abc const *abc)
{
    struct imm_mute_state *state = xmalloc(sizeof(*state));
    state->super = imm_state_new(id, abc, vtable, state);
    return state;
}

struct imm_state *imm_mute_state_read(FILE *stream, struct imm_abc const *abc)
{
    struct imm_state *state = __imm_state_read(stream, abc);
    if (!state)
    {
        error("could not read normal state");
        return NULL;
    }

    struct imm_mute_state *mute_state = xmalloc(sizeof(*mute_state));
    mute_state->super = state;
    state->vtable = vtable;
    state->derived = mute_state;

    return state;
}

static void del(struct imm_state const *state)
{
    free((void *)__imm_state_derived(state));
    __imm_state_free(state);
}

static imm_float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
    if (imm_seq_len(seq) == 0)
        return imm_log(1.0);
    return imm_lprob_zero();
}

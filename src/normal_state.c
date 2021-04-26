#include "imm/normal_state.h"
#include "common/common.h"
#include "imm/abc.h"
#include "imm/error.h"
#include "imm/seq.h"
#include "imm/state_types.h"
#include <stdlib.h>

static void del(struct imm_state const *state);
static imm_float lprob(struct imm_state const *state,
                       struct imm_seq const *seq);
static uint8_t min_seq(struct imm_state const *state) { return 1; }
static uint8_t max_seq(struct imm_state const *state) { return 1; }
static uint8_t type_id(struct imm_state const *state)
{
    return IMM_NORMAL_STATE_TYPE_ID;
}

static struct imm_state_vtable const vtable = {del, lprob, max_seq, min_seq,
                                               type_id};

struct imm_normal_state *imm_normal_state_new(uint16_t id,
                                              struct imm_abc const *abc,
                                              imm_float const *lprobs)
{
    struct imm_normal_state *state = xmalloc(sizeof(*state));

    uint8_t len = imm_abc_len(abc);
    state->lprobs = xmalloc(sizeof(*state->lprobs) * len);
    xmemcpy(state->lprobs, lprobs, sizeof(*state->lprobs) * len);

    state->super = imm_state_new(id, abc, vtable, state);
    return state;
}

struct imm_state *imm_normal_state_read(FILE *stream, struct imm_abc const *abc)
{
    struct imm_state *state = __imm_state_read(stream, abc);
    if (!state)
        return NULL;

    uint8_t lprobs_size = 0;
    imm_float *lprobs = NULL;

    if (fread(&lprobs_size, sizeof(lprobs_size), 1, stream) < 1)
    {
        xerror(IMM_IOERROR, "could not read lprobs_size");
        imm_state_free(state);
        return NULL;
    }

    lprobs = xmalloc(sizeof(*lprobs) * lprobs_size);

    if (fread(lprobs, sizeof(*lprobs), lprobs_size, stream) < lprobs_size)
    {
        xerror(IMM_IOERROR, "could not read lprobs");
        free(lprobs);
        imm_state_free(state);
        return NULL;
    }

    struct imm_normal_state *normal_state = xmalloc(sizeof(*normal_state));
    normal_state->super = state;
    normal_state->lprobs = lprobs;
    state->vtable = vtable;
    state->derived = normal_state;

    return state;
}

/* int imm_normal_state_write(struct imm_state const *state, */
/*                            struct imm_profile const *prof, FILE *stream) */
/* { */
/*     int error = IMM_SUCCESS; */
/*     if ((error = __imm_state_write(state, stream))) */
/*         return error; */

/*     struct imm_normal_state const *s = __imm_state_derived(state); */

/*     struct chunk chunk = CHUNK_INIT(imm_abc_length(state->abc), s->lprobs);
 */
/*     xfwrite(&chunk.lprobs_size, sizeof(chunk.lprobs_size), 1, stream); */
/*     xfwrite(chunk.lprobs, sizeof(*chunk.lprobs), chunk.lprobs_size, stream);
 */

/*     return error; */
/* } */

static void del(struct imm_state const *state)
{
    struct imm_normal_state const *s = __imm_state_derived(state);
    free(s->lprobs);
    free((void *)s);
    __imm_state_free(state);
}

static imm_float lprob(struct imm_state const *state, struct imm_seq const *seq)
{
    struct imm_normal_state const *s = __imm_state_derived(state);
    if (imm_seq_len(seq) == 1)
    {
        struct imm_abc const *abc = imm_state_abc(state);
        unsigned idx = imm_abc_symbol_idx(abc, imm_seq_str(seq)[0]);
        if (idx != IMM_ABC_INVALID_IDX)
            return s->lprobs[idx];
    }
    return imm_lprob_zero();
}

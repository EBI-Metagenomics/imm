#include "imm/state.h"
#include "common/common.h"
#include "imm/abc.h"

struct imm_state *imm_state_new(uint16_t id, struct imm_abc const *abc,
                                struct imm_state_vtable vtable, void *derived)
{
    if (imm_abc_len(abc) == 0)
    {
        error("empty alphabet");
        return NULL;
    }

    struct imm_state *state = xmalloc(sizeof(*state));
    state->id = id;
    state->abc = abc;
    state->vtable = vtable;
    state->derived = derived;
    stack_init(&state->trans);
    hnode_init(&state->hnode);
    return state;
}

struct imm_state *__imm_state_read(FILE *stream, struct imm_abc const *abc)
{
    struct imm_state *state = xmalloc(sizeof(*state));
    if (fread(&state->id, sizeof(state->id), 1, stream) < 1)
    {
        error("failed to read state id");
        free(state);
        return NULL;
    }

    state->abc = abc;
    state->vtable = (struct imm_state_vtable){NULL, NULL, NULL, NULL, NULL};
    state->derived = NULL;
    stack_init(&state->trans);
    hnode_init(&state->hnode);

    return state;
}

void __imm_state_write(struct imm_state const *state, FILE *stream)
{
    xfwrite(&state->id, sizeof(state->id), 1, stream);
}

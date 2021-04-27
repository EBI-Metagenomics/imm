#include "imm/state.h"
#include "common/common.h"
#include "imm/abc.h"
#include "imm/error.h"

struct imm_state *imm_state_new(uint16_t id, struct imm_abc const *abc,
                                struct imm_state_vtable vtable, void *derived)
{
    struct imm_state *state = xmalloc(sizeof(*state));
    state->id = id;
    state->abc = abc;
    state->vtable = vtable;
    state->derived = derived;
    stack_init(&state->trans);
    state->nitrans = 0;
    hnode_init(&state->hnode);
    return state;
}

struct imm_state *__imm_state_read(FILE *stream, struct imm_abc const *abc)
{
    struct imm_state *state = xmalloc(sizeof(*state));
    if (fread(&state->id, sizeof(state->id), 1, stream) < 1)
    {
        xerror(IMM_IOERROR, "failed to read state id");
        free(state);
        return NULL;
    }

    state->abc = abc;
    state->vtable = (struct imm_state_vtable){NULL, NULL, NULL, NULL, NULL};
    state->derived = NULL;
    stack_init(&state->trans);
    state->nitrans = 0; /* TODO: read nintras */
    hnode_init(&state->hnode);

    return state;
}

void __imm_state_write(struct imm_state const *state, FILE *stream)
{
    xfwrite(&state->id, sizeof(state->id), 1, stream);
}

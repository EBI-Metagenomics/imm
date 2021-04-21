#include "imm/imm.h"
#include "std.h"
#include <stdlib.h>
#include <string.h>

struct state_chunk
{
    uint16_t id;
    uint8_t  name_length;
    char*    name;
};

static char const unnamed[] = "unnamed";

static inline int ascii_is_std(char const* str, size_t len);

struct imm_state* imm_state_create(uint16_t id, char const* name, struct imm_abc const* abc,
                                   struct imm_state_vtable vtable, void* derived)
{
    if (imm_abc_length(abc) == 0) {
        error("empty alphabet");
        return NULL;
    }

    if (name && !ascii_is_std(name, strlen(name))) {
        error("name must be a string of non-extended ASCII characters");
        return NULL;
    }

    struct imm_state* s = xmalloc(sizeof(*s));
    s->id = id;
    s->name = name && *name != '\0' ? xstrdup(name) : NULL;
    s->abc = abc;
    s->vtable = vtable;
    s->derived = derived;
    stack_init(&s->trans);
    hnode_init(&s->hnode);
    return s;
}

imm_float imm_state_lprob(struct imm_state const* state, struct imm_seq const* seq)
{
    if (state->abc != imm_seq_get_abc(seq)) {
        error("alphabets must be the same");
        return imm_lprob_invalid();
    }
    return state->vtable.lprob(state, seq);
}

char const* imm_state_name(struct imm_state const* state) { return state->name ? state->name : unnamed; }

void imm_state_destroy(struct imm_state const* state) { state->vtable.destroy(state); }

void __imm_state_destroy(struct imm_state const* state)
{
    if (state->name)
        free((void*)state->name);
    free((void*)state);
}

struct imm_state* __imm_state_read(FILE* stream, struct imm_abc const* abc)
{
    struct state_chunk chunk = {0, 0, NULL};

    if (fread(&chunk.id, sizeof(chunk.id), 1, stream) < 1)
        return NULL;

    if (fread(&chunk.name_length, sizeof(chunk.name_length), 1, stream) < 1)
        return NULL;

    if (chunk.name_length > 0) {
        chunk.name = malloc(sizeof(*chunk.name) * (size_t)(chunk.name_length + 1));
        if (!chunk.name) {
            error_explain(IMM_OUTOFMEM);
            return NULL;
        }

        if (fread(chunk.name, sizeof(*chunk.name), (size_t)(chunk.name_length + 1), stream) <
            (size_t)(chunk.name_length + 1)) {
            free(chunk.name);
            return NULL;
        }
    }

    struct imm_state* state = xmalloc(sizeof(*state));
    state->id = chunk.id;
    state->name = chunk.name;
    state->abc = abc;
    state->vtable = (struct imm_state_vtable){NULL, NULL, NULL, NULL, NULL};
    state->derived = NULL;
    stack_init(&state->trans);
    hnode_init(&state->hnode);

    return state;
}

int __imm_state_write(struct imm_state const* state, FILE* stream)
{
    struct state_chunk chunk = {
        .id = state->id, .name_length = state->name ? (uint8_t)strlen(state->name) : 0, .name = (char*)state->name};

    xfwrite(&chunk.id, sizeof(chunk.id), 1, stream);
    xfwrite(&chunk.name_length, sizeof(chunk.name_length), 1, stream);
    if (chunk.name)
        xfwrite(chunk.name, sizeof(*chunk.name), chunk.name_length + 1, stream);

    return IMM_SUCCESS;
}

static inline int ascii_is_std(char const* str, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        signed int const a = str[i];
        if (a < 0)
            return 0;
    }
    return 1;
}

#include "imm/imm.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>

struct state_chunk
{
    uint8_t name_length;
    char*   name;
};

static inline int ascii_is_std(char const* str, size_t len);

struct imm_state const* imm_state_create(char const* name, struct imm_abc const* abc, struct imm_state_vtable vtable,
                                         void* derived)
{
    if (imm_abc_length(abc) == 0) {
        error("empty alphabet");
        return NULL;
    }

    if (!ascii_is_std(name, strlen(name))) {
        error("name must be a string of non-extended ASCII characters");
        return NULL;
    }

    struct imm_state* s = malloc(sizeof(*s));
    s->name = strdup(name);
    s->abc = abc;
    s->vtable = vtable;
    s->derived = derived;
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

void imm_state_destroy(struct imm_state const* state) { state->vtable.destroy(state); }

void __imm_state_destroy(struct imm_state const* state)
{
    free((void*)state->name);
    free((void*)state);
}

struct imm_state* __imm_state_read(FILE* stream, struct imm_abc const* abc)
{
    struct state_chunk chunk;

    if (fread(&chunk.name_length, sizeof(chunk.name_length), 1, stream) < 1) {
        return NULL;
    }

    chunk.name = malloc(sizeof(*chunk.name) * (size_t)(chunk.name_length + 1));

    if (fread(chunk.name, sizeof(*chunk.name), (size_t)(chunk.name_length + 1), stream) <
        (size_t)(chunk.name_length + 1)) {
        free(chunk.name);
        return NULL;
    }

    struct imm_state* state = malloc(sizeof(*state));
    state->name = chunk.name;
    state->abc = abc;
    state->vtable = (struct imm_state_vtable){NULL, NULL, NULL, NULL, NULL};
    state->derived = NULL;

    return state;
}

int __imm_state_write(struct imm_state const* state, FILE* stream)
{
    struct state_chunk chunk = {.name_length = (uint8_t)strlen(imm_state_get_name(state)),
                                .name = (char*)imm_state_get_name(state)};

    if (fwrite(&chunk.name_length, sizeof(chunk.name_length), 1, stream) < 1) {
        error("could not write name_length");
        return 1;
    }

    if (fwrite(chunk.name, sizeof(*chunk.name), (size_t)(chunk.name_length + 1), stream) <
        (size_t)(chunk.name_length + 1)) {
        error("could not write name");
        return 1;
    }

    return 0;
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

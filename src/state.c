#include "state.h"
#include "ascii.h"
#include "cast.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/report.h"
#include "imm/state.h"
#include <stdlib.h>
#include <string.h>

struct state_chunk
{
    uint16_t name_length;
    char*    name;
};

struct imm_state const* imm_state_create(char const* name, struct imm_abc const* abc,
                                         struct imm_state_vtable vtable, void* derived)
{
    if (imm_abc_length(abc) == 0) {
        imm_error("empty alphabet");
        return NULL;
    }

    if (!ascii_is_std(name, strlen(name))) {
        imm_error("name must be a string of non-extended ASCII characters");
        return NULL;
    }

    struct imm_state* s = malloc(sizeof(*s));
    s->name = strdup(name);
    s->abc = abc;
    s->vtable = vtable;
    s->derived = derived;
    return s;
}

void imm_state_destroy(struct imm_state const* state) { state->vtable.destroy(state); }

void __imm_state_destroy(struct imm_state const* state)
{
    free_c(state->name);
    free_c(state);
}

struct imm_state* __imm_state_read(FILE* stream, struct imm_abc const* abc)
{
    struct state_chunk chunk;

    if (fread(&chunk.name_length, sizeof(chunk.name_length), 1, stream) < 1) {
        return NULL;
    }

    chunk.name = malloc(sizeof(*chunk.name) * (chunk.name_length + 1));

    if (fread(chunk.name, sizeof(*chunk.name), chunk.name_length + 1, stream) <
        chunk.name_length + 1) {
        free_c(chunk.name);
        return NULL;
    }

    struct imm_state* state = malloc(sizeof(*state));
    state->name = chunk.name;
    state->abc = abc;
    state->vtable = (struct imm_state_vtable){NULL, NULL, NULL, NULL, NULL, NULL};
    state->derived = NULL;

    return state;
}

int __imm_state_write(struct imm_state const* state, FILE* stream)
{
    struct state_chunk chunk = {.name_length = cast_zu_u16(strlen(imm_state_get_name(state))),
                                .name = (char*)imm_state_get_name(state)};

    if (fwrite(&chunk.name_length, sizeof(chunk.name_length), 1, stream) < 1) {
        imm_error("could not write name_length");
        return 1;
    }

    if (fwrite(chunk.name, sizeof(*chunk.name), chunk.name_length + 1, stream) <
        chunk.name_length + 1) {
        imm_error("could not write name");
        return 1;
    }

    return 0;
}

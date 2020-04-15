#include "state_factory.h"
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
    uint8_t  state_type;
};

struct imm_state const* imm_state_create(char const* name, struct imm_abc const* abc,
                                         struct imm_state_funcs funcs, uint8_t type_id,
                                         void* impl)
{
    if (imm_abc_length(abc) == 0) {
        imm_error("empty alphabet");
        return NULL;
    }

    if (!ascii_is_std(name, strlen(name))) {
        imm_error("name must be a string of non-extended ASCII characters");
        return NULL;
    }

    struct imm_state* s = malloc(sizeof(struct imm_state));
    s->name = strdup(name);
    s->abc = abc;
    s->lprob = funcs.lprob;
    s->min_seq = funcs.min_seq;
    s->max_seq = funcs.max_seq;
    s->write = funcs.write;
    s->type_id = type_id;
    s->impl = impl;
    return s;
}

void imm_state_destroy(struct imm_state const* state)
{
    free_c(state->name);
    free_c(state);
}

int state_write(struct imm_state const* state, FILE* stream)
{
    struct state_chunk chunk = {.name_length = cast_u16_zu(strlen(imm_state_get_name(state))),
                                .name = (char*)imm_state_get_name(state),
                                .state_type = imm_state_type_id(state)};

    if (fwrite(&chunk.name_length, sizeof(chunk.name_length), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.name, sizeof(*chunk.name), chunk.name_length + 1, stream) <
        chunk.name_length + 1)
        return 1;

    if (fwrite(&chunk.state_type, sizeof(chunk.state_type), 1, stream) < 1)
        return 1;

    return state->write(state, stream);
}

struct imm_state const* state_read(FILE* stream, struct imm_abc const* abc)
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

    if (fread(&chunk.state_type, sizeof(chunk.state_type), 1, stream) < 1) {
        return NULL;
    }

    struct imm_state *state = malloc(sizeof(*state));
    state->name = chunk.name;
    state->abc = abc;
    state->type_id = chunk.state_type;

    if (state_factory_read(stream, state)) {
        imm_error("could not state_factory_read");
        free_c(state);
        return NULL;
    }

    return state;
}

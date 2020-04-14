#include "imm/state.h"
#include "ascii.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/report.h"
#include <stdlib.h>
#include <string.h>

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

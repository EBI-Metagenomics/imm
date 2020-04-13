#include "mstate.h"
#include "cast.h"
#include "free.h"
#include "imm/state.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include <limits.h>
#include <math.h>
#include <string.h>

struct mstate_chunk
{
    uint8_t     state_type;
    uint16_t    name_size;
    char const* name;
    double      start_lprob;
    uint32_t    impl_chunk_size;
    char*       impl_chunk;
};

struct mstates_chunk
{
    uint32_t            nstates;
    struct mstate_chunk mstate_chunk;
};

static int mstate_write(struct mstate const* mstate, FILE* stream);

struct mstate* mstate_create(struct imm_state const* state, double start_lprob)
{
    struct mstate* mstate = malloc(sizeof(struct mstate));
    mstate->state = state;
    mstate->start_lprob = start_lprob;
    mstate->mtrans_table = mtrans_table_create();
    return mstate;
}

void mstate_destroy(struct mstate* mstate)
{
    mtrans_table_destroy(mstate->mtrans_table);
    free_c(mstate);
}

double mstate_get_start(struct mstate const* mstate) { return mstate->start_lprob; }

void mstate_set_start(struct mstate* mstate, double const lprob)
{
    mstate->start_lprob = lprob;
}

struct mtrans_table* mstate_get_mtrans_table(struct mstate const* mstate)
{
    return mstate->mtrans_table;
}

int mstate_write_states(FILE* stream, struct mstate const* const* mstates, uint32_t nstates)
{
    struct mstates_chunk chunk = {.nstates = nstates};

    if (fwrite(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1)
        return 1;

    for (uint32_t i = 0; i < nstates; ++i) {
        if (mstate_write( mstates[i], stream))
            return 1;
    }

    return 0;
}

static int mstate_write( struct mstate const* mstate, FILE* stream)
{
    struct imm_state const* state = mstate_get_state(mstate);

    struct mstate_chunk chunk = {.state_type = 0,
                                 .name_size =
                                     (cast_u16_zu(strlen(imm_state_get_name(state))) + 1),
                                 .name = imm_state_get_name(state),
                                 .start_lprob = mstate_get_start(mstate),
                                 .impl_chunk_size = 0,
                                 .impl_chunk = NULL};

    if (fwrite(&chunk.state_type, sizeof(chunk.state_type), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.name_size, sizeof(chunk.name_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.name, sizeof(*chunk.name), chunk.name_size, stream) < chunk.name_size)
        return 1;

    if (fwrite(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.impl_chunk_size, sizeof(chunk.impl_chunk_size), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.impl_chunk, sizeof(*chunk.impl_chunk), chunk.impl_chunk_size, stream) <
        chunk.impl_chunk_size)
        return 1;

    return 0;
}

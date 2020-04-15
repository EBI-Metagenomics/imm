#include "mstate.h"
#include "cast.h"
#include "free.h"
#include "imm/state.h"
#include "io.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "state.h"
#include <limits.h>
#include <math.h>
#include <string.h>

struct mstate_chunk
{
    double start_lprob;
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
        if (mstate_write(mstates[i], stream))
            return 1;
    }

    return 0;
}

int mstate_read_states(FILE* stream, struct imm_io* io)
{
    /* TODO: fix memory leak */
    uint32_t nstates = 0;

    if (fread(&nstates, sizeof(nstates), 1, stream) < 1) {
        imm_error("could not read nstates");
        return 1;
    }

    io->mstates = malloc(sizeof(*io->mstates) * nstates);
    for (uint32_t i = 0; i < nstates; ++i)
        io->mstates[i] = NULL;

    for (uint32_t i = 0; i < nstates; ++i) {

        struct mstate_chunk chunk;

        if (fread(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1) {
            imm_error("could not read start_lprob");
            return 1;
        }

        struct imm_state const* state = state_read(stream, io->abc);
        if (!state) {
            imm_error("could not read state");
            return 1;
        }
    }

    return 0;
}

static int mstate_write(struct mstate const* mstate, FILE* stream)
{
    struct mstate_chunk chunk = {mstate_get_start(mstate)};

    if (fwrite(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1) {
        imm_error("could not write start_lprob");
        return 1;
    }

    struct imm_state const* state = mstate_get_state(mstate);
    if (state_write(state, stream)) {
        imm_error("could not write state");
        return 1;
    }

    return 0;
}

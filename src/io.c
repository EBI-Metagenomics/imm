#include "imm/io.h"
#include "dp.h"
#include "free.h"
#include "hmm.h"
#include "imm/abc.h"
#include "imm/hmm.h"
#include "imm/report.h"
#include "io.h"
#include "mstate.h"
#include <stdlib.h>

int imm_io_write(FILE* stream, struct imm_hmm const* hmm, struct imm_dp const* dp)
{
    if (hmm_write(hmm, dp, stream)) {
        imm_error("could not write hmm");
        return 1;
    }

    if (dp_write(dp, stream)) {
        imm_error("could not write dp");
        return 1;
    }

    return 0;
}

struct imm_io const* imm_io_read(FILE* stream)
{
    struct imm_io* io = malloc(sizeof(*io));
    io->abc = NULL;
    io->hmm = NULL;
    io->mstates = NULL;
    io->nstates = 0;

    if (hmm_read(stream, io)) {
        imm_error("could not read hmm");
        goto err;
    }

    if (dp_read(stream, io)) {
        imm_error("could not read dp");
        goto err;
    }

    return io;

err:
    if (io->abc)
        imm_abc_destroy(io->abc);

    if (io->hmm)
        imm_hmm_destroy(io->hmm);

    if (io->mstates)
        free_c(io->mstates);

    free_c(io);
    return NULL;
}

void imm_io_destroy(struct imm_io const* io)
{
    if (io->mstates)
        free_c(io->mstates);
    free_c(io);
}

struct imm_state const* imm_io_state(struct imm_io const* io, uint32_t i)
{
    return mstate_get_state(io->mstates[i]);
}

uint32_t imm_io_nstates(struct imm_io const* io) { return io->nstates; }

struct imm_abc const* imm_io_abc(struct imm_io const* io) { return io->abc; }

struct imm_hmm* imm_io_hmm(struct imm_io const* io) { return io->hmm; }

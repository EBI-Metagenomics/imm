#include "imm/io.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_trans_table.h"
#include "free.h"
#include "hmm.h"
#include "imm/abc.h"
#include "imm/hmm.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"
#include "imm/report.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "io.h"
#include "mstate.h"
#include "seq_code.h"
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
    io->states = NULL;
    io->nstates = 0;
    io->seq_code = NULL;
    io->emission = NULL;
    io->trans_table = NULL;
    io->state_table = NULL;

    if (hmm_read(stream, io)) {
        imm_error("could not read hmm");
        goto err;
    }

    if (dp_read(stream, io)) {
        imm_error("could not read dp");
        goto err;
    }

    dp_create_from_io(io);

    return io;

err:
    if (io->abc)
        imm_abc_destroy(io->abc);

    if (io->hmm)
        imm_hmm_destroy(io->hmm);

    if (io->mstates)
        free_c(io->mstates);

    if (io->seq_code)
        seq_code_destroy(io->seq_code);

    if (io->emission)
        dp_emission_destroy(io->emission);

    if (io->trans_table)
        dp_trans_table_destroy(io->trans_table);

    free_c(io);
    return NULL;
}

void imm_io_destroy(struct imm_io const* io)
{
    free_c(io->states);
    free_c(io);
}

void imm_io_destroy_states(struct imm_io const* io)
{
    for (uint32_t i = 0; i < imm_io_nstates(io); ++i)
        imm_state_destroy(imm_io_state(io, i));
}

struct imm_state const* imm_io_state(struct imm_io const* io, uint32_t i)
{
    return io->states[i];
}

uint32_t imm_io_nstates(struct imm_io const* io) { return io->nstates; }

struct imm_abc const* imm_io_abc(struct imm_io const* io) { return io->abc; }

struct imm_hmm* imm_io_hmm(struct imm_io const* io) { return io->hmm; }

struct imm_dp const* imm_io_dp(struct imm_io const* io) { return io->dp; }

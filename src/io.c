#include "imm/io.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "free.h"
#include "hmm.h"
#include "imm/abc.h"
#include "imm/abc_types.h"
#include "imm/hmm.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"
#include "imm/report.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "io.h"
#include "mstate.h"
#include "mstate_table.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "mute_state.h"
#include "normal_state.h"
#include "seq_code.h"
#include <stdlib.h>

static int write_abc(struct imm_abc const* abc, FILE* stream, uint8_t type_id);
static int write_abc_chunk(struct imm_io const* io, struct imm_abc const* abc, FILE* stream);
static struct imm_abc const* read_abc(FILE* stream, uint8_t type_id);
static int                   hmm_read(FILE* stream, struct imm_io* io);
static int                   hmm_write(struct imm_dp const* dp, FILE* stream);
static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct mstate* const* const mstates);

static struct imm_io_vtable const vtable = {read_abc, write_abc};

struct imm_io const* imm_io_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    struct imm_io* io = malloc(sizeof(*io));

    io->abc = hmm_abc(hmm);
    io->hmm = hmm;
    io->mstates = dp_get_mstates(dp);

    io->nstates = dp_state_table_nstates(dp_get_state_table(dp));
    io->states = malloc(sizeof(*io->states) * io->nstates);
    for (uint32_t i = 0; i < io->nstates; ++i)
        io->states[i] = io->mstates[i]->state;

    io->seq_code = dp_get_seq_code(dp);
    io->emission = dp_get_emission(dp);
    io->trans_table = dp_get_trans_table(dp);
    io->state_table = dp_get_state_table(dp);
    io->dp = dp;
    io->vtable = vtable;

    return io;
}

struct imm_io const* imm_io_create_from_file(FILE* stream)
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
    io->vtable = vtable;

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

void imm_io_destroy_states(struct imm_io const* io)
{
    for (uint32_t i = 0; i < imm_io_nstates(io); ++i)
        imm_state_destroy(imm_io_state(io, i));
}

void imm_io_destroy(struct imm_io const* io)
{
    free_c(io->states);
    free_c(io);
}

int imm_io_write(struct imm_io const* io, FILE* stream)
{
    if (write_abc_chunk(io, io->abc, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    if (hmm_write(io->dp, stream)) {
        imm_error("could not write hmm");
        return 1;
    }

    if (dp_write(io->dp, stream)) {
        imm_error("could not write dp");
        return 1;
    }

    return 0;
}

struct imm_state const* imm_io_state(struct imm_io const* io, uint32_t i) { return io->states[i]; }

uint32_t imm_io_nstates(struct imm_io const* io) { return io->nstates; }

struct imm_abc const* imm_io_abc(struct imm_io const* io) { return io->abc; }

struct imm_hmm* imm_io_hmm(struct imm_io const* io) { return io->hmm; }

struct imm_dp const* imm_io_dp(struct imm_io const* io) { return io->dp; }

int io_read_abc(struct imm_io* io, FILE* stream)
{
    uint8_t type_id = 0;
    if (fread(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not read type_id");
        return 1;
    }

    io->abc = io->vtable.read_abc(stream, type_id);

    return !io->abc;
}

struct imm_state const* io_read_state(FILE* stream, uint8_t type_id, struct imm_abc const* abc)
{
    struct imm_state const* state = NULL;

    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        if (!(state = mute_state_read(stream, abc)))
            imm_error("could not read mute_state");
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (!(state = normal_state_read(stream, abc)))
            imm_error("could not read normal_state");
        break;
    default:
        imm_error("unknown type_id");
    }
    return state;
}

static int write_abc(struct imm_abc const* abc, FILE* stream, uint8_t type_id)
{
    if (type_id != IMM_ABC_TYPE_ID) {
        imm_error("unknown abc type_id");
        return 1;
    }

    if (imm_abc_write(abc, stream)) {
        imm_error("could not write abc");
        return 1;
    }
    return 0;
}

static int write_abc_chunk(struct imm_io const* io, struct imm_abc const* abc, FILE* stream)
{
    uint8_t type_id = imm_abc_type_id(abc);
    if (fwrite(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not write type_id");
        return 1;
    }

    if (io->vtable.write_abc(abc, stream, type_id)) {
        imm_error("could not write abc");
        return 1;
    }
    return 0;
}

static struct imm_abc const* read_abc(FILE* stream, uint8_t type_id)
{
    if (type_id != IMM_ABC_TYPE_ID) {
        imm_error("unknown abc type_id");
        return NULL;
    }

    struct imm_abc const* abc = imm_abc_read(stream);
    if (!abc) {
        imm_error("could not read abc");
        return abc;
    }
    return abc;
}

static int hmm_read(FILE* stream, struct imm_io* io)
{
    struct imm_hmm* hmm = NULL;
    io->mstates = NULL;

    if (io_read_abc(io, stream)) {
        imm_error("could not read abc");
        goto err;
    }

    hmm = imm_hmm_create(io->abc);

    if (!(io->mstates = mstate_read_states(stream, &io->nstates, io->abc))) {
        imm_error("could not read states");
        goto err;
    }

    io->states = malloc(sizeof(*io->states) * io->nstates);

    for (uint32_t i = 0; i < io->nstates; ++i) {
        io->states[i] = io->mstates[i]->state;
        hmm_add_mstate(hmm, io->mstates[i]);
    }

    if (read_transitions(stream, hmm, io->mstates)) {
        imm_error("could not read transitions");
        goto err;
    }

    io->hmm = hmm;

    return 0;

err:
    if (io->abc)
        imm_abc_destroy(io->abc);

    if (hmm)
        imm_hmm_destroy(hmm);

    return 1;
}

static int hmm_write(struct imm_dp const* dp, FILE* stream)
{
    struct dp_state_table const* state_tbl = dp_get_state_table(dp);
    if (mstate_write_states(stream, dp_get_mstates(dp), dp_state_table_nstates(state_tbl))) {
        imm_error("could not write states");
        return 1;
    }

    struct dp_trans_table const* trans_tbl = dp_get_trans_table(dp);
    uint32_t                     ntrans = dp_trans_table_total_ntrans(trans_tbl);
    if (fwrite(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        imm_error("could not write ntrans");
        return 1;
    }

    for (uint32_t tgt_state = 0; tgt_state < dp_state_table_nstates(state_tbl); ++tgt_state) {

        ntrans = dp_trans_table_ntrans(trans_tbl, tgt_state);
        for (uint32_t trans = 0; trans < ntrans; ++trans) {

            uint32_t src_state = dp_trans_table_source_state(trans_tbl, tgt_state, trans);
            double   lprob = dp_trans_table_score(trans_tbl, tgt_state, trans);

            if (fwrite(&src_state, sizeof(src_state), 1, stream) < 1) {
                imm_error("could not write source_state");
                return 1;
            }

            if (fwrite(&tgt_state, sizeof(tgt_state), 1, stream) < 1) {
                imm_error("could not write target_state");
                return 1;
            }

            if (fwrite(&lprob, sizeof(lprob), 1, stream) < 1) {
                imm_error("could not write lprob");
                return 1;
            }
        }
    }

    return 0;
}

static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct mstate* const* const mstates)
{
    uint32_t ntrans = 0;

    if (fread(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        imm_error("could not read ntransitions");
        return 1;
    }

    for (uint32_t i = 0; i < ntrans; ++i) {

        uint32_t src_state = 0;
        uint32_t tgt_state = 0;
        double   lprob = 0;

        if (fread(&src_state, sizeof(src_state), 1, stream) < 1) {
            imm_error("could not read source_state");
            return 1;
        }

        if (fread(&tgt_state, sizeof(tgt_state), 1, stream) < 1) {
            imm_error("could not read target_state");
            return 1;
        }

        if (fread(&lprob, sizeof(lprob), 1, stream) < 1) {
            imm_error("could not read lprob");
            return 1;
        }

        struct mtrans_table*    tbl = mstate_get_mtrans_table(mstates[src_state]);
        struct imm_state const* tgt = mstate_get_state(mstates[tgt_state]);
        mtrans_table_add(tbl, mtrans_create(tgt, lprob));
    }

    return 0;
}

#include "imm/model.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "free.h"
#include "hmm.h"
#include "imm/abc.h"
#include "imm/abc_types.h"
#include "imm/dp.h"
#include "imm/hmm.h"
#include "imm/report.h"
#include "imm/state_types.h"
#include "model.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "mute_state.h"
#include "normal_state.h"
#include "seq_code.h"
#include "state.h"
#include <stdlib.h>
#include <string.h>

struct mstate_chunk
{
    double  start_lprob;
    uint8_t type_id;
};

struct mstates_chunk
{
    uint32_t            nstates;
    struct mstate_chunk mstate_chunk;
};

static struct mstate** read_mstates(struct imm_model* model, FILE* stream);
static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct mstate* const* const mstates);
static int write(struct imm_model const* model, FILE* stream);
static int write_mstate(struct imm_model const* model, FILE* stream, struct mstate const* mstate);
static int write_mstates(struct imm_model const* model, FILE* stream,
                         struct mstate const* const* mstates, uint32_t nstates);

static struct imm_model_vtable const __model_vtable = {
    imm_model_destroy,
    __imm_model_destroy_on_read_failure,
    __imm_model_read_state,
    write,
};

struct imm_abc const* imm_model_abc(struct imm_model const* model) { return model->abc; }

struct imm_model* imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    return __imm_model_create(hmm, dp, NULL);
}

void imm_model_destroy(struct imm_model const* model)
{
    free_c(model->states);
    free_c(model);
}

struct imm_dp const* imm_model_dp(struct imm_model const* model) { return model->dp; }

struct imm_hmm* imm_model_hmm(struct imm_model const* model) { return model->hmm; }

uint32_t imm_model_nstates(struct imm_model const* model) { return model->nstates; }

struct imm_state const* imm_model_state(struct imm_model const* model, uint32_t i)
{
    return model->states[i];
}

struct imm_model* __imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp, void* derived)
{
    struct imm_model* model = malloc(sizeof(*model));

    model->abc = hmm_abc(hmm);
    model->hmm = hmm;
    model->mstates = (struct mstate**)hmm_get_mstates(hmm, dp);

    model->nstates = dp_state_table_nstates(dp_get_state_table(dp));
    model->states = malloc(sizeof(*model->states) * model->nstates);
    for (uint32_t i = 0; i < model->nstates; ++i)
        model->states[i] = model->mstates[i]->state;

    model->seq_code = dp_get_seq_code(dp);
    model->emission = dp_get_emission(dp);
    model->trans_table = dp_get_trans_table(dp);
    model->state_table = dp_get_state_table(dp);
    model->dp = dp;

    model->vtable = __model_vtable;
    model->derived = NULL;

    return model;
}

void* __imm_model_derived(struct imm_model const* model) { return model->derived; }

void __imm_model_destroy_on_read_failure(struct imm_model const* model)
{
    if (model->abc)
        imm_abc_destroy(model->abc);

    if (model->hmm)
        imm_hmm_destroy(model->hmm);

    if (model->mstates)
        free_c(model->mstates);

    if (model->states)
        free_c(model->states);

    if (model->seq_code)
        seq_code_destroy(model->seq_code);

    if (model->emission)
        dp_emission_destroy(model->emission);

    if (model->trans_table)
        dp_trans_table_destroy(model->trans_table);

    if (model->state_table)
        dp_state_table_destroy(model->state_table);

    if (model->dp)
        imm_dp_destroy(model->dp);

    free_c(model);
}

struct imm_model* __imm_model_new(void* derived)
{
    struct imm_model* model = malloc(sizeof(*model));
    model->abc = NULL;
    model->hmm = NULL;
    model->mstates = NULL;
    model->nstates = 0;
    model->states = NULL;
    model->seq_code = NULL;
    model->emission = NULL;
    model->trans_table = NULL;
    model->state_table = NULL;
    model->dp = NULL;

    model->vtable = __model_vtable;
    model->derived = derived;

    return model;
}

int __imm_model_read(struct imm_model* model, FILE* stream)
{
    if (__imm_model_read_abc(model, stream)) {
        imm_error("could not read abc");
        goto err;
    }

    if (__imm_model_read_hmm(model, stream)) {
        imm_error("could not read hmm");
        goto err;
    }

    if (__imm_model_read_dp(model, stream)) {
        imm_error("could not read dp");
        goto err;
    }

    __imm_dp_create_from_io(model);
    return 0;

err:
    model->vtable.destroy_on_read_failure(model);
    return 1;
}

int __imm_model_read_abc(struct imm_model* model, FILE* stream)
{
    uint8_t type_id = 0;
    if (fread(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not read type_id");
        return 1;
    }

    if (type_id != IMM_ABC_TYPE_ID) {
        imm_error("unknown abc type_id");
        return 1;
    }

    model->abc = imm_abc_read(stream);
    if (!model->abc) {
        imm_error("could not read abc");
        return 1;
    }

    return 0;
}

int __imm_model_read_dp(struct imm_model* model, FILE* stream)
{
    if (!(model->seq_code = seq_code_read(stream, model->abc))) {
        imm_error("could not read seq_code");
        return 1;
    }

    if (!(model->emission = dp_emission_read(stream))) {
        imm_error("could not read dp_emission");
        return 1;
    }

    if (!(model->trans_table = dp_trans_table_read(stream))) {
        imm_error("could not read dp_trans_table");
        return 1;
    }

    if (!(model->state_table = dp_state_table_read(stream))) {
        imm_error("could not read dp_state_table");
        return 1;
    }

    return 0;
}

int __imm_model_read_hmm(struct imm_model* model, FILE* stream)
{
    struct imm_hmm* hmm = NULL;
    model->mstates = NULL;

    hmm = imm_hmm_create(model->abc);

    if (!(model->mstates = read_mstates(model, stream))) {
        imm_error("could not read states");
        goto err;
    }

    model->states = malloc(sizeof(*model->states) * model->nstates);

    for (uint32_t i = 0; i < model->nstates; ++i) {
        model->states[i] = model->mstates[i]->state;
        hmm_add_mstate(hmm, model->mstates[i]);
    }

    if (read_transitions(stream, hmm, model->mstates)) {
        imm_error("could not read transitions");
        goto err;
    }

    model->hmm = hmm;

    return 0;

err:
    if (hmm)
        imm_hmm_destroy(hmm);

    return 1;
}

struct imm_state const* __imm_model_read_state(struct imm_model const* model, FILE* stream,
                                               uint8_t type_id)
{
    struct imm_state const* state = NULL;

    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        if (!(state = mute_state_read(stream, model->abc)))
            imm_error("could not read mute_state");
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (!(state = normal_state_read(stream, model->abc)))
            imm_error("could not read normal_state");
        break;
    default:
        imm_error("unknown state type_id");
    }

    return state;
}

void __imm_model_set_abc(struct imm_model* model, struct imm_abc const* abc) { model->abc = abc; }

struct imm_model_vtable* __imm_model_vtable(struct imm_model* model) { return &model->vtable; }

int __imm_model_write_abc(struct imm_model const* model, FILE* stream)
{
    uint8_t type_id = imm_abc_type_id(model->abc);
    if (fwrite(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not write type_id");
        return 1;
    }

    if (imm_abc_write(model->abc, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    return 0;
}

int __imm_model_write_dp(struct imm_model const* model, FILE* stream)
{
    if (seq_code_write(model->seq_code, stream)) {
        imm_error("could not write seq_code");
        return 1;
    }

    if (dp_emission_write(model->emission, model->nstates, stream)) {
        imm_error("could not write dp_emission");
        return 1;
    }

    if (dp_trans_table_write(model->trans_table, model->nstates, stream)) {
        imm_error("could not write dp_trans_table");
        return 1;
    }

    if (dp_state_table_write(model->state_table, stream)) {
        imm_error("could not write dp_state_table");
        return 1;
    }

    return 0;
}

int __imm_model_write_hmm(struct imm_model const* model, FILE* stream)
{
    if (write_mstates(model, stream, (struct mstate const* const*)model->mstates, model->nstates)) {
        imm_error("could not write states");
        return 1;
    }

    uint32_t ntrans = dp_trans_table_total_ntrans(model->trans_table);
    if (fwrite(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        imm_error("could not write ntrans");
        return 1;
    }

    for (uint32_t tgt_state = 0; tgt_state < model->nstates; ++tgt_state) {

        ntrans = dp_trans_table_ntrans(model->trans_table, tgt_state);
        for (uint32_t trans = 0; trans < ntrans; ++trans) {

            uint32_t src_state = dp_trans_table_source_state(model->trans_table, tgt_state, trans);
            double   score = dp_trans_table_score(model->trans_table, tgt_state, trans);

            if (fwrite(&src_state, sizeof(src_state), 1, stream) < 1) {
                imm_error("could not write source_state");
                return 1;
            }

            if (fwrite(&tgt_state, sizeof(tgt_state), 1, stream) < 1) {
                imm_error("could not write target_state");
                return 1;
            }

            if (fwrite(&score, sizeof(score), 1, stream) < 1) {
                imm_error("could not write score");
                return 1;
            }
        }
    }

    return 0;
}

static struct mstate** read_mstates(struct imm_model* model, FILE* stream)
{
    struct mstate** mstates = NULL;

    if (fread(&model->nstates, sizeof(model->nstates), 1, stream) < 1) {
        imm_error("could not read nstates");
        goto err;
    }

    mstates = malloc(sizeof(*mstates) * (model->nstates));
    for (uint32_t i = 0; i < model->nstates; ++i)
        mstates[i] = NULL;

    for (uint32_t i = 0; i < model->nstates; ++i) {

        struct mstate_chunk chunk = {0., 0};

        if (fread(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1) {
            imm_error("could not read start_lprob");
            goto err;
        }

        if (fread(&chunk.type_id, sizeof(chunk.type_id), 1, stream) < 1) {
            imm_error("could not read type_id");
            goto err;
        }

        struct imm_state const* state = model->vtable.read_state(model, stream, chunk.type_id);
        if (!state) {
            imm_error("could not read state");
            goto err;
        }

        mstates[i] = mstate_create(state, chunk.start_lprob);
    }

    return mstates;

err:
    if (mstates) {
        for (uint32_t i = 0; i < model->nstates; ++i) {
            if (mstates[i])
                mstate_destroy(mstates[i]);
        }

        free_c(mstates);
    }

    return NULL;
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

static int write(struct imm_model const* model, FILE* stream)
{
    if (__imm_model_write_abc(model, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    if (__imm_model_write_hmm(model, stream)) {
        imm_error("could not write hmm");
        return 1;
    }

    if (__imm_model_write_dp(model, stream)) {
        imm_error("could not write dp");
        return 1;
    }

    return 0;
}

static int write_mstate(struct imm_model const* model, FILE* stream, struct mstate const* mstate)
{
    struct imm_state const* state = mstate_get_state(mstate);

    struct mstate_chunk chunk = {mstate_get_start(mstate), imm_state_type_id(state)};

    if (fwrite(&chunk.start_lprob, sizeof(chunk.start_lprob), 1, stream) < 1) {
        imm_error("could not write start_lprob");
        return 1;
    }

    if (fwrite(&chunk.type_id, sizeof(chunk.type_id), 1, stream) < 1) {
        imm_error("could not write type_id");
        return 1;
    }

    if (state_write(state, model, stream)) {
        imm_error("could not write state");
        return 1;
    }

    return 0;
}

static int write_mstates(struct imm_model const* model, FILE* stream,
                         struct mstate const* const* mstates, uint32_t nstates)
{
    struct mstates_chunk chunk = {.nstates = nstates};

    if (fwrite(&chunk.nstates, sizeof(chunk.nstates), 1, stream) < 1) {
        imm_error("could not write nstates");
        return 1;
    }

    for (uint32_t i = 0; i < nstates; ++i) {
        if (write_mstate(model, stream, mstates[i])) {
            imm_error("could not write mstate");
            return 1;
        }
    }

    return 0;
}

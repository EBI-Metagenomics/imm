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
#include "imm/mute_state.h"
#include "imm/normal_state.h"
#include "imm/report.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "imm/table_state.h"
#include "model.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "seq_code.h"
#include <stdlib.h>
#include <string.h>

static int                  read_abc(struct imm_model* model, FILE* stream);
static struct model_state** read_mstates(struct imm_model* model, FILE* stream);
static int                  read_transitions(FILE* stream, struct imm_hmm* hmm,
                                             struct model_state* const* const mstates);
static int                  write_abc(struct imm_model const* model, FILE* stream);
static int                  write_mstate(struct imm_model const* model, FILE* stream,
                                         struct model_state const* mstate);
static int                  write_mstates(struct imm_model const* model, FILE* stream,
                                          struct model_state const* const* mstates, uint16_t nstates);

struct imm_abc const* imm_model_abc(struct imm_model const* model) { return model->abc; }

struct imm_model* imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    return __imm_model_create(hmm, dp, __imm_model_read_state, NULL, __imm_model_write_state, NULL);
}

void imm_model_destroy(struct imm_model const* model)
{
    free_c(model->states);
    free_c(model);
}

struct imm_dp const* imm_model_dp(struct imm_model const* model) { return model->dp; }

struct imm_hmm* imm_model_hmm(struct imm_model const* model) { return model->hmm; }

uint16_t imm_model_nstates(struct imm_model const* model) { return model->nstates; }

struct imm_model const* imm_model_read(FILE* stream)
{
    struct imm_model* model =
        __imm_model_new(__imm_model_read_state, NULL, __imm_model_write_state, NULL);

    if (read_abc(model, stream)) {
        imm_error("could not read abc");
        goto err;
    }

    uint8_t nhmms = 0;
    if (fread(&nhmms, sizeof(nhmms), 1, stream) < 1) {
        imm_error("could not read nhmms");
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

    return model;

err:
    __imm_model_deep_destroy(model);
    return NULL;
}

struct imm_state const* imm_model_state(struct imm_model const* model, uint16_t i)
{
    return model->states[i];
}

int imm_model_write(struct imm_model const* model, FILE* stream)
{
    if (write_abc(model, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    uint8_t nhmms = 1;
    if (fwrite(&nhmms, sizeof(nhmms), 1, stream) < 1) {
        imm_error("could not write nhmms");
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

struct imm_model* __imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp,
                                     imm_model_read_state_cb read_state, void* read_state_args,
                                     imm_model_write_state_cb write_state, void* write_state_args)
{
    struct imm_model* model = malloc(sizeof(*model));

    model->abc = hmm_abc(hmm);
    model->hmm = hmm;
    model->mstates = (struct model_state**)hmm_get_mstates(hmm, dp);

    model->nstates = (uint16_t)dp_state_table_nstates(dp_get_state_table(dp));
    model->states = malloc(sizeof(*model->states) * model->nstates);
    for (uint16_t i = 0; i < model->nstates; ++i)
        model->states[i] = model->mstates[i]->state;

    model->seq_code = dp_get_seq_code(dp);
    model->emission = dp_get_emission(dp);
    model->trans_table = dp_get_trans_table(dp);
    model->state_table = dp_get_state_table(dp);
    model->dp = dp;
    model->read_state = read_state;
    model->read_state_args = read_state_args;
    model->write_state = write_state;
    model->write_state_args = write_state_args;

    return model;
}

struct imm_model* __imm_model_new(imm_model_read_state_cb read_state, void* read_state_args,
                                  imm_model_write_state_cb write_state, void* write_state_args)
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
    model->read_state = read_state;
    model->read_state_args = read_state_args;
    model->write_state = write_state;
    model->write_state_args = write_state_args;

    return model;
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

    dp_create_from_model(model);

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

    for (uint16_t i = 0; i < model->nstates; ++i) {
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
                                               void* args)
{
    struct imm_state const* state = NULL;
    uint8_t                 type_id = 0;

    if (fread(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not read type id");
        return NULL;
    }

    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        if (!(state = imm_mute_state_read(stream, model->abc)))
            imm_error("could not read mute state");
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (!(state = imm_normal_state_read(stream, model->abc)))
            imm_error("could not read normal state");
        break;
    case IMM_TABLE_STATE_TYPE_ID:
        if (!(state = imm_table_state_read(stream, model->abc)))
            imm_error("could not read table state");
        break;
    default:
        imm_error("unknown state type id");
    }

    return state;
}

void __imm_model_set_abc(struct imm_model* model, struct imm_abc const* abc) { model->abc = abc; }

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
    if (write_mstates(model, stream, (struct model_state const* const*)model->mstates,
                      model->nstates)) {
        imm_error("could not write states");
        return 1;
    }

    uint16_t ntrans = (uint16_t)dp_trans_table_total_ntrans(model->trans_table);
    if (fwrite(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        imm_error("could not write ntrans");
        return 1;
    }

    for (uint16_t tgt_state = 0; tgt_state < model->nstates; ++tgt_state) {

        uint_fast16_t n = dp_trans_table_ntrans(model->trans_table, tgt_state);
        for (uint_fast16_t trans = 0; trans < n; ++trans) {

            uint16_t src_state =
                (uint16_t)dp_trans_table_source_state(model->trans_table, tgt_state, trans);
            score_t score = dp_trans_table_score(model->trans_table, tgt_state, trans);

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

int __imm_model_write_state(struct imm_model const* model, FILE* stream,
                            struct imm_state const* state, void* args)
{
    uint8_t type_id = imm_state_type_id(state);
    if (fwrite(&type_id, sizeof(type_id), 1, stream) < 1) {
        imm_error("could not write state type id");
        return 1;
    }

    int errno = 0;
    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        errno = imm_mute_state_write(state, model, stream);
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        errno = imm_normal_state_write(state, model, stream);
        break;
    case IMM_TABLE_STATE_TYPE_ID:
        errno = imm_table_state_write(state, model, stream);
        break;
    default:
        imm_error("unknown state type id");
        errno = 1;
    }
    return errno;
}

void __imm_model_deep_destroy(struct imm_model const* model)
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

static int read_abc(struct imm_model* model, FILE* stream)
{
    if (!(model->abc = imm_abc_read(stream))) {
        imm_error("could not read abc");
        return 1;
    }
    return 0;
}

static struct model_state** read_mstates(struct imm_model* model, FILE* stream)
{
    struct model_state** mstates = NULL;

    if (fread(&model->nstates, sizeof(model->nstates), 1, stream) < 1) {
        imm_error("could not read nstates");
        goto err;
    }

    mstates = malloc(sizeof(*mstates) * (model->nstates));
    for (uint16_t i = 0; i < model->nstates; ++i)
        mstates[i] = NULL;

    for (uint16_t i = 0; i < model->nstates; ++i) {

        score_t start_lprob = 0.;
        if (fread(&start_lprob, sizeof(start_lprob), 1, stream) < 1) {
            imm_error("could not read start lprob");
            goto err;
        }

        struct imm_state const* state = model->read_state(model, stream, model->read_state_args);
        if (!state) {
            imm_error("could not read state");
            goto err;
        }

        mstates[i] = model_state_create(state, start_lprob);
    }

    return mstates;

err:
    if (mstates) {
        for (uint16_t i = 0; i < model->nstates; ++i) {
            if (mstates[i])
                model_state_destroy(mstates[i]);
        }

        free_c(mstates);
    }

    return NULL;
}

static int read_transitions(FILE* stream, struct imm_hmm* hmm,
                            struct model_state* const* const mstates)
{
    uint16_t ntrans = 0;

    if (fread(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        imm_error("could not read ntransitions");
        return 1;
    }

    for (uint16_t i = 0; i < ntrans; ++i) {

        uint16_t src_state = 0;
        uint16_t tgt_state = 0;
        score_t  lprob = 0;

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

        struct model_trans_table* tbl = model_state_get_mtrans_table(mstates[src_state]);
        struct imm_state const*   tgt = model_state_get_state(mstates[tgt_state]);
        model_trans_table_add(tbl, model_trans_create(tgt, lprob));
    }

    return 0;
}

static int write_abc(struct imm_model const* model, FILE* stream)
{
    if (imm_abc_write(model->abc, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    return 0;
}

static int write_mstate(struct imm_model const* model, FILE* stream,
                        struct model_state const* mstate)
{
    struct imm_state const* state = model_state_get_state(mstate);

    score_t start_lprob = (score_t)model_state_get_start(mstate);

    if (fwrite(&start_lprob, sizeof(start_lprob), 1, stream) < 1) {
        imm_error("could not write start_lprob");
        return 1;
    }

    if (model->write_state(model, stream, state, model->write_state_args)) {
        imm_error("could not write state");
        return 1;
    }

    return 0;
}

static int write_mstates(struct imm_model const* model, FILE* stream,
                         struct model_state const* const* mstates, uint16_t nstates)
{
    if (fwrite(&nstates, sizeof(nstates), 1, stream) < 1) {
        imm_error("could not write nstates");
        return 1;
    }

    for (uint16_t i = 0; i < nstates; ++i) {
        if (write_mstate(model, stream, mstates[i])) {
            imm_error("could not write mstate");
            return 1;
        }
    }

    return 0;
}

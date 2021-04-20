#include "model.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "hmm.h"
#include "imm/imm.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "profile.h"
#include "seq_code.h"
#include "std.h"
#include <stdlib.h>

static struct imm_model*    new_model(void);
static int                  read_dp(struct imm_profile* prof, struct imm_model* model, FILE* stream);
static int                  read_hmm(struct imm_profile* prof, struct imm_model* model, FILE* stream);
static struct model_state** read_mstates(struct imm_profile* prof, struct imm_model* model, FILE* stream);
static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct model_state* const* const mstates);
static int write_dp(struct imm_model const* model, FILE* stream);
static int write_hmm(struct imm_profile const* prof, struct imm_model const* model, FILE* stream);
static int write_mstate(struct imm_profile const* prof, FILE* stream, struct model_state const* mstate);
static int write_mstates(struct imm_profile const* prof, FILE* stream, struct model_state const* const* mstates,
                         uint16_t nstates);

struct imm_model* imm_model_create(struct imm_hmm* hmm, struct imm_dp const* dp)
{
    struct imm_model* model = new_model();
    model->hmm = hmm;
    model->mstates = (struct model_state**)hmm_get_mstates(hmm, dp);

    model->nstates = (uint16_t)dp_state_table_nstates(dp_get_state_table(dp));
    model->states = malloc(sizeof(*model->states) * model->nstates);
    if (!model->states) {
        error_explain(IMM_OUTOFMEM);
        model_deep_destroy(model);
        return NULL;
    }
    for (uint16_t i = 0; i < model->nstates; ++i)
        model->states[i] = model->mstates[i]->state;

    model->seq_code = dp_get_seq_code(dp);
    model->emission = dp_get_emission(dp);
    model->trans_table = dp_get_trans_table(dp);
    model->state_table = dp_get_state_table(dp);
    model->dp = dp;
    return model;
}

/* TODO: remove this cast, implement imm_model_set_trans... */
struct imm_dp* imm_model_dp(struct imm_model const* model) { return (struct imm_dp*)model->dp; }

struct imm_hmm* imm_model_hmm(struct imm_model const* model) { return model->hmm; }

uint16_t imm_model_nstates(struct imm_model const* model) { return model->nstates; }

struct imm_state const* imm_model_state(struct imm_model const* model, uint16_t i) { return model->states[i]; }

void model_deep_destroy(struct imm_model const* model)
{
    if (model->hmm)
        imm_hmm_destroy(model->hmm);

    free_if(model->mstates);
    free_if(model->states);

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
}

void imm_model_destroy(struct imm_model const* model, bool deep)
{
    if (deep) {
        imm_hmm_destroy(imm_model_hmm(model));
        imm_dp_destroy(imm_model_dp(model));
        for (uint16_t i = 0; i < imm_model_nstates(model); ++i) {
            imm_state_destroy(imm_model_state(model, i));
        }
    }
    free(model->states);
    free((void*)model);
}

struct imm_model const* model_read(struct imm_profile* prof, FILE* stream)
{
    struct imm_model* model = new_model();

    if (read_hmm(prof, model, stream)) {
        error("could not read hmm");
        goto err;
    }

    if (read_dp(prof, model, stream)) {
        error("could not read dp");
        goto err;
    }
    return model;

err:
    model_deep_destroy(model);
    return NULL;
}

struct imm_state const* model_read_state(struct imm_profile* prof, FILE* stream)
{
    struct imm_state const* state = NULL;
    uint8_t                 type_id = 0;

    if (fread(&type_id, sizeof(type_id), 1, stream) < 1) {
        error("could not read type id");
        return NULL;
    }

    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        if (!(state = imm_mute_state_read(stream, prof->abc)))
            error("could not read mute state");
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (!(state = imm_normal_state_read(stream, prof->abc)))
            error("could not read normal state");
        break;
    case IMM_TABLE_STATE_TYPE_ID:
        if (!(state = imm_table_state_read(stream, prof->abc)))
            error("could not read table state");
        break;
    default:
        error("unknown state type id");
    }

    return state;
}

int model_write(struct imm_profile const* prof, struct imm_model const* model, FILE* stream)
{
    if (write_hmm(prof, model, stream)) {
        error("could not write hmm");
        return 1;
    }

    if (write_dp(model, stream)) {
        error("could not write dp");
        return 1;
    }

    return 0;
}

int model_write_state(struct imm_profile const* prof, FILE* stream, struct imm_state const* state)
{
    uint8_t type_id = imm_state_type_id(state);
    if (fwrite(&type_id, sizeof(type_id), 1, stream) < 1) {
        error("could not write state type id");
        return 1;
    }

    int errno = 0;
    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        errno = imm_mute_state_write(state, prof, stream);
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        errno = imm_normal_state_write(state, prof, stream);
        break;
    case IMM_TABLE_STATE_TYPE_ID:
        errno = imm_table_state_write(state, prof, stream);
        break;
    default:
        error("unknown state type id");
        errno = 1;
    }
    return errno;
}

static struct imm_model* new_model(void)
{
    struct imm_model* model = xmalloc(sizeof(*model));
    model->hmm = NULL;
    model->mstates = NULL;
    model->nstates = 0;
    model->states = NULL;
    model->seq_code = NULL;
    model->emission = NULL;
    model->trans_table = NULL;
    model->state_table = NULL;
    model->dp = NULL;
    return model;
}

static int read_dp(struct imm_profile* prof, struct imm_model* model, FILE* stream)
{
    if (!(model->seq_code = seq_code_read(stream, prof->abc))) {
        error("could not read seq_code");
        return 1;
    }

    if (!(model->emission = dp_emission_read(stream))) {
        error("could not read dp_emission");
        return 1;
    }

    if (!(model->trans_table = dp_trans_table_read(stream))) {
        error("could not read dp_trans_table");
        return 1;
    }

    if (!(model->state_table = dp_state_table_read(stream))) {
        error("could not read dp_state_table");
        return 1;
    }

    dp_create_from_model(model);

    return 0;
}

static int read_hmm(struct imm_profile* prof, struct imm_model* model, FILE* stream)
{
    model->hmm = imm_hmm_create(prof->abc);

    if (fread(&model->hmm->start_lprob, sizeof(model->hmm->start_lprob), 1, stream) < 1) {
        error("could not read starting lprob");
        goto err;
    }

    if (fread(&model->hmm->start_state, sizeof(model->hmm->start_state), 1, stream) < 1) {
        error("could not read starting state");
        goto err;
    }

    if (!(model->mstates = read_mstates(prof, model, stream))) {
        error("could not read states");
        goto err;
    }

    model->states = malloc(sizeof(*model->states) * model->nstates);
    if (!model->states) {
        error_explain(IMM_OUTOFMEM);
        model_deep_destroy(model);
        return IMM_OUTOFMEM;
    }

    for (uint16_t i = 0; i < model->nstates; ++i) {
        model->states[i] = model->mstates[i]->state;
        hmm_add_mstate(model->hmm, model->mstates[i]);
    }

    if (read_transitions(stream, model->hmm, model->mstates)) {
        error("could not read transitions");
        goto err;
    }

    return IMM_SUCCESS;

err:
    if (model->hmm) {
        imm_hmm_destroy(model->hmm);
        model->hmm = NULL;
    }

    return IMM_SUCCESS;
}

static struct model_state** read_mstates(struct imm_profile* prof, struct imm_model* model, FILE* stream)
{
    struct model_state** mstates = NULL;

    if (fread(&model->nstates, sizeof(model->nstates), 1, stream) < 1) {
        error("could not read nstates");
        goto err;
    }

    mstates = malloc(sizeof(*mstates) * (model->nstates));
    if (!mstates)
        goto err;

    for (uint16_t i = 0; i < model->nstates; ++i)
        mstates[i] = NULL;

    for (uint16_t i = 0; i < model->nstates; ++i) {

        imm_float start_lprob = 0.;
        if (fread(&start_lprob, sizeof(start_lprob), 1, stream) < 1) {
            error("could not read start lprob");
            goto err;
        }

        struct imm_state const* state = prof->vtable.read_state(prof, stream);
        if (!state) {
            error("could not read state");
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

        free(mstates);
    }

    return NULL;
}

static int read_transitions(FILE* stream, struct imm_hmm* hmm, struct model_state* const* const mstates)
{
    uint16_t ntrans = 0;

    if (fread(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        error("could not read ntransitions");
        return 1;
    }

    hmm->ntrans = 0;
    for (uint16_t i = 0; i < ntrans; ++i) {

        uint16_t  src_state = 0;
        uint16_t  tgt_state = 0;
        imm_float lprob = 0;

        if (fread(&src_state, sizeof(src_state), 1, stream) < 1) {
            error("could not read source_state");
            return 1;
        }

        if (fread(&tgt_state, sizeof(tgt_state), 1, stream) < 1) {
            error("could not read target_state");
            return 1;
        }

        if (fread(&lprob, sizeof(lprob), 1, stream) < 1) {
            error("could not read lprob");
            return 1;
        }
        struct model_trans_table* tbl = model_state_get_mtrans_table(mstates[src_state]);
        struct imm_state*         src = (struct imm_state*)model_state_get_state(mstates[src_state]);
        struct imm_state const*   tgt = model_state_get_state(mstates[tgt_state]);

        struct trans* newt = hmm->trans + hmm->ntrans++;
        trans_init(newt);
        newt->pair.ids[0] = src->id;
        newt->pair.ids[1] = tgt->id;
        newt->lprob = lprob;
        hash_add(hmm->trans_tbl, &newt->hnode, newt->pair.key);
        stack_put(&src->trans, &newt->node);

        model_trans_table_add(tbl, model_trans_create(tgt, lprob));
    }

    return 0;
}

static int write_dp(struct imm_model const* model, FILE* stream)
{
    if (seq_code_write(model->seq_code, stream)) {
        error("could not write seq_code");
        return 1;
    }

    if (dp_emission_write(model->emission, model->nstates, stream)) {
        error("could not write dp_emission");
        return 1;
    }

    if (dp_trans_table_write(model->trans_table, model->nstates, stream)) {
        error("could not write dp_trans_table");
        return 1;
    }

    if (dp_state_table_write(model->state_table, stream)) {
        error("could not write dp_state_table");
        return 1;
    }

    return 0;
}

static int write_hmm(struct imm_profile const* prof, struct imm_model const* model, FILE* stream)
{
    imm_float start_lprob = model->hmm->start_lprob;
    if (fwrite(&start_lprob, sizeof(start_lprob), 1, stream) < 1) {
        error("could not write starting lprob");
        return 1;
    }
    uint16_t start_state = model->hmm->start_state;
    if (fwrite(&start_state, sizeof(start_state), 1, stream) < 1) {
        error("could not write starting state id");
        return 1;
    }

    if (write_mstates(prof, stream, (struct model_state const* const*)model->mstates, model->nstates)) {
        error("could not write states");
        return 1;
    }

    uint16_t ntrans = (uint16_t)dp_trans_table_total_ntrans(model->trans_table);
    if (fwrite(&ntrans, sizeof(ntrans), 1, stream) < 1) {
        error("could not write ntrans");
        return 1;
    }

    for (uint16_t tgt_state = 0; tgt_state < model->nstates; ++tgt_state) {

        uint_fast16_t n = dp_trans_table_ntrans(model->trans_table, tgt_state);
        for (uint_fast16_t trans = 0; trans < n; ++trans) {

            uint16_t  src_state = (uint16_t)dp_trans_table_source_state(model->trans_table, tgt_state, trans);
            imm_float score = dp_trans_table_score(model->trans_table, tgt_state, trans);

            if (fwrite(&src_state, sizeof(src_state), 1, stream) < 1) {
                error("could not write source_state");
                return 1;
            }

            if (fwrite(&tgt_state, sizeof(tgt_state), 1, stream) < 1) {
                error("could not write target_state");
                return 1;
            }

            if (fwrite(&score, sizeof(score), 1, stream) < 1) {
                error("could not write score");
                return 1;
            }
        }
    }

    return 0;
}

static int write_mstate(struct imm_profile const* prof, FILE* stream, struct model_state const* mstate)
{
    struct imm_state const* state = model_state_get_state(mstate);

    imm_float start_lprob = (imm_float)model_state_get_start(mstate);

    if (fwrite(&start_lprob, sizeof(start_lprob), 1, stream) < 1) {
        error("could not write start_lprob");
        return 1;
    }

    if (prof->vtable.write_state(prof, stream, state)) {
        error("could not write state");
        return 1;
    }

    return 0;
}

static int write_mstates(struct imm_profile const* prof, FILE* stream, struct model_state const* const* mstates,
                         uint16_t nstates)
{
    if (fwrite(&nstates, sizeof(nstates), 1, stream) < 1) {
        error("could not write nstates");
        return 1;
    }

    for (uint16_t i = 0; i < nstates; ++i) {
        if (write_mstate(prof, stream, mstates[i])) {
            error("could not write mstate");
            return 1;
        }
    }

    return 0;
}

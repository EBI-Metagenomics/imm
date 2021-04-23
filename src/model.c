#include "model.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "hmm.h"
#include "imm/error.h"
#include "imm/imm.h"
#include "imm/log.h"
#include "profile.h"
#include "seq_code.h"
#include "std.h"
#include <stdint.h>
#include <stdlib.h>

static struct imm_model *new_model(void);
static int read_dp(struct imm_profile *prof, struct imm_model *model,
                   FILE *stream, struct imm_state **states);
static struct imm_state **read_hmm(struct imm_profile *prof,
                                   struct imm_model *model, FILE *stream);
static struct imm_state **read_states(struct imm_profile *prof,
                                      struct imm_model *model, FILE *stream,
                                      uint16_t *nstates);
static int read_transitions(FILE *stream, struct imm_hmm *hmm,
                            struct imm_state **states);
static int write_dp(struct imm_model const *model, FILE *stream);
static int write_hmm(struct imm_profile const *prof,
                     struct imm_model const *model, FILE *stream);
static int write_state(struct imm_profile const *prof, FILE *stream,
                       struct imm_state *state);
static int write_states(struct imm_profile const *prof, FILE *stream,
                        struct imm_model const *model);

struct imm_model *imm_model_create(struct imm_hmm *hmm, struct imm_dp *dp)
{
    struct imm_model *model = new_model();
    model->hmm = hmm;
    model->dp = dp;
    model->seq_code = dp_get_seq_code(dp);
    model->emission = dp_get_emission(dp);
    model->trans_table = dp_get_trans_table(dp);
    model->state_table = dp_get_state_table(dp);
    return model;
}

struct imm_dp *imm_model_dp(struct imm_model const *model) { return model->dp; }

struct imm_hmm *imm_model_hmm(struct imm_model const *model)
{
    return model->hmm;
}

uint16_t imm_model_nstates(struct imm_model const *model)
{
    return model->hmm->nstates;
}

struct imm_state *imm_model_state(struct imm_model const *model, uint16_t i)
{
    return hmm_get_states(model->hmm, model->dp)[i];
}

void imm_model_destroy(struct imm_model const *model, bool deep)
{
    if (deep)
    {
        if (model->hmm)
            imm_hmm_destroy(model->hmm);

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
    free((void *)model);
}

struct imm_model const *model_read(struct imm_profile *prof, FILE *stream)
{
    struct imm_model *model = new_model();

    if (!read_hmm(prof, model, stream))
    {
        error("could not read hmm");
        goto err;
    }

    if (read_dp(prof, model, stream, states))
    {
        error("could not read dp");
        goto err;
    }
    return model;

err:
    imm_model_destroy(model, true);
    return NULL;
}

struct imm_state *model_read_state(struct imm_profile *prof, FILE *stream)
{
    struct imm_state *state = NULL;
    uint8_t type_id = 0;

    if (fread(&type_id, sizeof(type_id), 1, stream) < 1)
    {
        error("could not read type id");
        return NULL;
    }

    switch (type_id)
    {
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

int model_write(struct imm_profile const *prof, struct imm_model const *model,
                FILE *stream)
{
    int error = IMM_SUCCESS;
    if ((error = write_hmm(prof, model, stream)))
    {
        error("could not write hmm");
        return error;
    }

    if ((error = write_dp(model, stream)))
    {
        error("could not write dp");
        return error;
    }

    return error;
}

int model_write_state(struct imm_profile const *prof, FILE *stream,
                      struct imm_state const *state)
{
    uint8_t type_id = imm_state_type_id(state);
    xfwrite(&type_id, sizeof(type_id), 1, stream);

    int errno = 0;
    switch (type_id)
    {
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

static struct imm_model *new_model(void)
{
    struct imm_model *model = xmalloc(sizeof(*model));
    model->hmm = NULL;
    model->seq_code = NULL;
    model->emission = NULL;
    model->trans_table = NULL;
    model->state_table = NULL;
    model->dp = NULL;
    return model;
}

static int read_dp(struct imm_profile *prof, struct imm_model *model,
                   FILE *stream, struct imm_state **states)
{
    struct seq_code const *seq_code = NULL;
    struct dp_emission const *emission = NULL;
    struct dp_trans_table *trans_table = NULL;
    struct dp_state_table const *state_table = NULL;

    if (!(seq_code = seq_code_read(stream, prof->abc)))
    {
        error("could not read seq_code");
        return 1;
    }

    if (!(emission = dp_emission_read(stream)))
    {
        error("could not read dp_emission");
        return 1;
    }

    if (!(trans_table = dp_trans_table_read(stream)))
    {
        error("could not read dp_trans_table");
        return 1;
    }

    if (!(state_table = dp_state_table_read(stream)))
    {
        error("could not read dp_state_table");
        return 1;
    }

    model->dp =
        dp_create_from(states, seq_code, emission, trans_table, state_table);
    return IMM_SUCCESS;

err:
    return IMM_IOERROR;
}

static struct imm_state **read_hmm(struct imm_profile *prof,
                                   struct imm_model *model, FILE *stream)
{
    struct imm_state **states = NULL;
    model->hmm = imm_hmm_create(prof->abc);

    if (fread(&model->hmm->start_lprob, sizeof(model->hmm->start_lprob), 1,
              stream) < 1)
    {
        error("could not read starting lprob");
        goto err;
    }

    if (fread(&model->hmm->start_state, sizeof(model->hmm->start_state), 1,
              stream) < 1)
    {
        error("could not read starting state");
        goto err;
    }

    uint16_t nstates = model->hmm->nstates;
    if (!(states = read_states(prof, model, stream, &nstates)))
    {
        error("could not read states");
        goto err;
    }

    for (uint16_t i = 0; i < nstates; ++i)
        hmm_add_state(model->hmm, states[i]);

    if (read_transitions(stream, model->hmm, states))
    {
        error("could not read transitions");
        goto err;
    }

    return states;

err:
    if (model->hmm)
    {
        imm_hmm_destroy(model->hmm);
        model->hmm = NULL;
    }
    return NULL;
}

static struct imm_state **read_states(struct imm_profile *prof,
                                      struct imm_model *model, FILE *stream,
                                      uint16_t *nstates)
{
    struct imm_state **states = NULL;

    *nstates = 0;
    if (fread(nstates, sizeof(*nstates), 1, stream) < 1)
    {
        error("could not read number of states");
        goto err;
    }

    states = xmalloc(sizeof(*states) * (*nstates));
    for (unsigned i = 0; i < (*nstates); ++i)
        states[i] = NULL;

    for (unsigned i = 0; i < (*nstates); ++i)
    {

        states[i] = prof->vtable.read_state(prof, stream);
        if (!states[i])
        {
            error("could not read %u-th state", i);
            goto err;
        }
    }

    return states;

err:
    if (states)
    {
        for (unsigned i = 0; i < (*nstates); ++i)
        {
            if (states[i])
                imm_state_destroy(states[i]);
        }
        free(states);
    }

    return NULL;
}

static int read_transitions(FILE *stream, struct imm_hmm *hmm,
                            struct imm_state **states)
{
    uint16_t ntrans = 0;

    if (fread(&ntrans, sizeof(ntrans), 1, stream) < 1)
    {
        error("could not read ntransitions");
        return 1;
    }

    for (uint16_t i = 0; i < ntrans; ++i)
    {

        uint16_t src_state = 0;
        uint16_t dst_state = 0;
        imm_float lprob = 0.;

        if (fread(&src_state, sizeof(src_state), 1, stream) < 1)
        {
            error("could not read source state");
            return IMM_IOERROR;
        }

        if (fread(&dst_state, sizeof(dst_state), 1, stream) < 1)
        {
            error("could not read destination state");
            return IMM_IOERROR;
        }

        if (fread(&lprob, sizeof(lprob), 1, stream) < 1)
        {
            error("could not read transition lprob");
            return IMM_IOERROR;
        }
        struct imm_state *src = states[src_state];
        struct imm_state const *dst = states[dst_state];
        imm_hmm_set_trans(hmm, src, dst, lprob);
    }
    BUG(hmm->ntrans != ntrans);
    return IMM_SUCCESS;
}

static int write_dp(struct imm_model const *model, FILE *stream)
{
    if (seq_code_write(model->seq_code, stream))
    {
        error("could not write seq_code");
        return 1;
    }

    if (dp_emission_write(model->emission, model->nstates, stream))
    {
        error("could not write dp_emission");
        return 1;
    }

    if (dp_trans_table_write(model->trans_table, model->nstates, stream))
    {
        error("could not write dp_trans_table");
        return 1;
    }

    if (dp_state_table_write(model->state_table, stream))
    {
        error("could not write dp_state_table");
        return 1;
    }

    return 0;
}

static int write_hmm(struct imm_profile const *prof,
                     struct imm_model const *model, FILE *stream)
{
    imm_float start_lprob = model->hmm->start_lprob;
    xfwrite(&start_lprob, sizeof(start_lprob), 1, stream);

    uint16_t start_state = model->hmm->start_state;
    xfwrite(&start_state, sizeof(start_state), 1, stream);

    if (write_states(prof, stream, model))
    {
        error("could not write states");
        return IMM_IOERROR;
    }

    uint16_t ntrans = model->hmm->ntrans;
    xfwrite(&ntrans, sizeof(ntrans), 1, stream);

    for (uint16_t dst = 0; dst < model->hmm->nstates; ++dst)
    {

        unsigned n = dp_trans_table_ntrans(model->trans_table, dst);
        for (unsigned trans = 0; trans < n; ++trans)
        {

            uint16_t src = (uint16_t)dp_trans_table_source_state(
                model->trans_table, dst, trans);
            imm_float score =
                dp_trans_table_score(model->trans_table, dst, trans);

            uint16_t src_id = dp_get_states(model->dp)[src]->id;
            uint16_t dst_id = dp_get_states(model->dp)[dst]->id;

            xfwrite(&src_id, sizeof(src_id), 1, stream);
            xfwrite(&dst_id, sizeof(dst_id), 1, stream);
            xfwrite(&score, sizeof(score), 1, stream);
        }
    }

    return IMM_SUCCESS;
}

static int write_state(struct imm_profile const *prof, FILE *stream,
                       struct imm_state *state)
{
    if (prof->vtable.write_state(prof, stream, state))
    {
        error("could not write state");
        return IMM_IOERROR;
    }
    return IMM_SUCCESS;
}

static int write_states(struct imm_profile const *prof, FILE *stream,
                        struct imm_model const *model)
{
    uint16_t nstates = model->hmm->nstates;
    xfwrite(&nstates, sizeof(nstates), 1, stream);

    for (uint16_t i = 0; i < nstates; ++i)
    {
        if (write_state(prof, stream, dp_get_states(model->dp)[i]))
        {
            error("could not write state");
            return IMM_IOERROR;
        }
    }
    return IMM_SUCCESS;
}

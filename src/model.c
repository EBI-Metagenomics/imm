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
#include "imm/hmm_block.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"
#include "imm/report.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "imm/table_state.h"
#include "model.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "seq_code.h"
#include <stdlib.h>
#include <string.h>

static int read_abc(struct imm_model* model, FILE* stream);
static int write_abc(struct imm_model const* model, FILE* stream);

struct imm_abc const* imm_model_abc(struct imm_model const* model) { return model->abc; }

struct imm_model* imm_model_create(struct imm_abc const* abc)
{
    return __imm_model_create(abc, __imm_hmm_block_read_state, NULL, __imm_hmm_block_write_state,
                              NULL);
}

void imm_model_destroy(struct imm_model const* model)
{
    free_c(model->hmm_blocks);
    free_c(model);
}

struct imm_model const* imm_model_read(FILE* stream)
{
    struct imm_model* model =
        __imm_model_new(__imm_hmm_block_read_state, NULL, __imm_hmm_block_write_state, NULL);

    if (read_abc(model, stream)) {
        imm_error("could not read abc");
        goto err;
    }

    if (fread(&model->nhmm_blocks, sizeof(model->nhmm_blocks), 1, stream) < 1) {
        imm_error("could not read hmm blocks");
        goto err;
    }

    model->hmm_blocks = malloc(sizeof(*model->hmm_blocks) * model->nhmm_blocks);
    for (uint8_t i = 0; i < model->nhmm_blocks; ++i)
        model->hmm_blocks[i] = NULL;

    for (uint8_t i = 0; i < model->nhmm_blocks; ++i) {

        if (__imm_hmm_block_read_hmm(model->hmm_blocks[i], model->abc, model->read_state,
                                     model->read_state_args, stream)) {
            imm_error("could not read hmm");
            goto err;
        }

        if (__imm_model_read_dp(model, stream)) {
            imm_error("could not read dp");
            goto err;
        }
    }

    return model;

err:
    __imm_model_deep_destroy(model);
    return NULL;
}

int imm_model_write(struct imm_model const* model, FILE* stream)
{
    if (write_abc(model, stream)) {
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

struct imm_model* __imm_model_create(struct imm_abc const*       abc,
                                     imm_hmm_block_read_state_cb read_state, void* read_state_args,
                                     imm_hmm_block_write_state_cb write_state,
                                     void*                        write_state_args)
{
    struct imm_model* model = malloc(sizeof(*model));

    model->abc = abc;
    model->nhmm_blocks = 0;
    model->hmm_blocks = NULL;

    model->read_state = read_state;
    model->read_state_args = read_state_args;
    model->write_state = write_state;
    model->write_state_args = write_state_args;

    return model;
}

struct imm_model* __imm_model_new(imm_hmm_block_read_state_cb read_state, void* read_state_args,
                                  imm_hmm_block_write_state_cb write_state, void* write_state_args)
{
    struct imm_model* model = malloc(sizeof(*model));
    model->abc = NULL;
    model->nhmm_blocks = 0;
    model->hmm_blocks = NULL;
    model->read_state = read_state;
    model->read_state_args = read_state_args;
    model->write_state = write_state;
    model->write_state_args = write_state_args;
    return model;
}

void __imm_model_set_abc(struct imm_model* model, struct imm_abc const* abc) { model->abc = abc; }

void __imm_model_deep_destroy(struct imm_model const* model)
{
    if (model->abc)
        imm_abc_destroy(model->abc);

    for (uint8_t i = 0; i < model->nhmm_blocks; ++i)
        __imm_hmm_block_deep_destroy(model->hmm_blocks[i]);

    free_c(model->hmm_blocks);
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

static int write_abc(struct imm_model const* model, FILE* stream)
{
    if (imm_abc_write(model->abc, stream)) {
        imm_error("could not write abc");
        return 1;
    }

    return 0;
}

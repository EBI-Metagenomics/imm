#include "profile.h"
#include "dp.h"
#include "dp_emission.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"
#include "hmm.h"
#include "imm/imm.h"
#include "model.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "std.h"
#include <stdlib.h>
#include <string.h>

static int write_abc(struct imm_profile const* prof, FILE* stream);

struct imm_abc const* imm_profile_abc(struct imm_profile const* prof) { return prof->abc; }

void imm_profile_append_model(struct imm_profile* prof, struct imm_model* model)
{
    BUG(prof->abc != hmm_abc(model->hmm));
    imm_vecp_append(prof->models, model);
}

struct imm_profile* imm_profile_create(struct imm_abc const* abc)
{
    return __imm_profile_create(abc, (struct imm_profile_vtable){model_read_state, model_write_state}, NULL);
}

void imm_profile_destroy(struct imm_profile const* prof, bool deep)
{
    for (size_t i = 0; i < imm_profile_nmodels(prof); ++i) {
        struct imm_model* model = (void*)imm_vecp_get(prof->models, i);
        imm_model_destroy(model, deep);
    }
    if (deep)
        imm_abc_destroy(imm_profile_abc(prof));
    imm_vecp_destroy(prof->models);
    free((void*)prof);
}

void imm_profile_free(struct imm_profile const* prof)
{
    imm_vecp_destroy(prof->models);
    free((void*)prof);
}

struct imm_model* imm_profile_get_model(struct imm_profile const* prof, uint8_t i)
{
    return (void*)imm_vecp_get(prof->models, i);
}

uint8_t imm_profile_nmodels(struct imm_profile const* prof) { return (uint8_t)imm_vecp_size(prof->models); }

struct imm_profile const* imm_profile_read(FILE* stream)
{
    struct imm_abc const* abc = imm_abc_read(stream);
    struct imm_profile*   prof = NULL;
    if (!abc) {
        error("could not read abc");
        goto err;
    }
    prof = __imm_profile_create(abc, (struct imm_profile_vtable){model_read_state, model_write_state}, NULL);

    if (__imm_profile_read_models(prof, stream))
        goto err;

    return prof;

err:
    if (prof)
        __imm_profile_deep_destroy(prof);
    return NULL;
}

int imm_profile_write(struct imm_profile const* prof, FILE* stream)
{
    if (write_abc(prof, stream)) {
        error("could not write abc");
        return 1;
    }

    if (__imm_profile_write_models(prof, stream))
        return 1;

    return 0;
}

struct imm_profile* __imm_profile_create(struct imm_abc const* abc, struct imm_profile_vtable vtable, void* derived)
{
    struct imm_profile* prof = xmalloc(sizeof(*prof));
    prof->abc = abc;
    prof->models = imm_vecp_create();
    prof->vtable = vtable;
    prof->derived = derived;
    return prof;
}

void __imm_profile_deep_destroy(struct imm_profile const* prof)
{
    if (prof->abc)
        imm_abc_destroy(prof->abc);

    for (uint8_t i = 0; i < imm_profile_nmodels(prof); ++i) {
        struct imm_model* model = (void*)imm_vecp_get(prof->models, 0);
        model_deep_destroy(model);
    }
    imm_vecp_destroy(prof->models);
    free((void*)prof);
}
void* __imm_profile_derived(struct imm_profile* prof) { return prof->derived; }

void const* __imm_profile_derived_c(struct imm_profile const* prof) { return prof->derived; }

int __imm_profile_read_models(struct imm_profile* prof, FILE* stream)
{
    uint8_t nhmms = 0;
    if (fread(&nhmms, sizeof(nhmms), 1, stream) < 1) {
        error("could not read nhmms");
        return 1;
    }

    for (uint8_t i = 0; i < nhmms; ++i) {

        struct imm_model const* model = model_read(prof, stream);
        if (!model)
            return 1;
        imm_vecp_append(prof->models, model);
    }
    return 0;
}
void __imm_profile_set_abc(struct imm_profile* prof, struct imm_abc const* abc) { prof->abc = abc; }

int __imm_profile_write_models(struct imm_profile const* prof, FILE* stream)
{
    uint8_t nhmms = imm_profile_nmodels(prof);
    if (fwrite(&nhmms, sizeof(nhmms), 1, stream) < 1) {
        error("could not write nhmms");
        return 1;
    }

    for (uint8_t i = 0; i < nhmms; ++i) {

        struct imm_model* model = (void*)imm_vecp_get(prof->models, i);
        if (model_write(prof, model, stream))
            return 1;
    }

    return 0;
}

static int write_abc(struct imm_profile const* prof, FILE* stream)
{
    if (imm_abc_write(prof->abc, stream)) {
        error("could not write abc");
        return 1;
    }

    return 0;
}

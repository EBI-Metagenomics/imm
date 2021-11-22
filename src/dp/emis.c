#include "dp/emis.h"
#include "code.h"
#include "error.h"
#include "imm/abc.h"
#include "imm/cartes.h"
#include "imm/code.h"
#include "imm/state.h"
#include <assert.h>
#include <stdlib.h>

void emis_del(struct imm_dp_emis const *emis)
{
    free(emis->score);
    free(emis->offset);
}

void emis_init(struct imm_dp_emis *emis)
{
    emis->score = NULL;
    emis->offset = NULL;
}

enum imm_rc emis_reset(struct imm_dp_emis *emis, struct imm_code const *code,
                       struct imm_state **states, unsigned nstates)
{
    unsigned offsize = emis_offset_size(nstates);
    emis->offset = realloc(emis->offset, sizeof(*emis->offset) * offsize);
    if (!emis->offset && offsize > 0)
        return error(IMM_OUTOFMEM, "failed to resize");
    emis->offset[0] = 0;

    unsigned min = imm_state_span(states[0]).min;
    unsigned max = imm_state_span(states[0]).max;
    unsigned size = code_size(code, min, max);
    for (unsigned i = 1; i < nstates; ++i)
    {
        emis->offset[i] = size;
        min = imm_state_span(states[i]).min;
        max = imm_state_span(states[i]).max;
        size += code_size(code, min, max);
    }
    emis->offset[nstates] = size;

    unsigned score_size = emis_score_size(emis, nstates);
    emis->score = realloc(emis->score, sizeof(*emis->score) * score_size);
    if (!emis->score && score_size > 0)
    {
        free(emis->offset);
        emis->offset = NULL;
        return error(IMM_OUTOFMEM, "failed to resize");
    }

    struct imm_abc const *abc = code->abc;
    char const *set = abc->symbols;
    unsigned set_size = abc->size;
    struct imm_cartes cartes;
    /* TODO: consider to avoid initing this everytime */
    imm_cartes_init(&cartes, set, set_size, IMM_STATE_MAX_SEQLEN);

    for (unsigned i = 0; i < nstates; ++i)
    {
        min = imm_state_span(states[i]).min;
        max = imm_state_span(states[i]).max;
        for (unsigned len = min; len <= max; ++len)
        {
            imm_cartes_setup(&cartes, len);
            char const *item = NULL;
            while ((item = imm_cartes_next(&cartes)) != NULL)
            {
                struct imm_seq seq = IMM_SEQ_UNSAFE(len, item, abc);
                unsigned c = code_translate(code, code_encode(code, &seq), min);
                imm_float score = imm_state_lprob(states[i], &seq);
                assert(!imm_lprob_is_nan(score));
                emis->score[emis->offset[i] + c] = score;
            }
        }
    }
    imm_cartes_deinit(&cartes);
    return IMM_SUCCESS;
}

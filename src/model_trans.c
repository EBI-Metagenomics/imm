#include "model_trans.h"
#include "bug.h"
#include "free.h"

struct model_trans
{
    struct imm_state const* state;
    imm_float               lprob;
};

struct model_trans* model_trans_create(struct imm_state const* state, imm_float lprob)
{
    struct model_trans* mtrans = malloc(sizeof(*mtrans));
    mtrans->state = state;
    mtrans->lprob = lprob;
    return mtrans;
}

void model_trans_destroy(struct model_trans const* mtrans)
{
    BUG(mtrans == NULL);
    free_c(mtrans);
}

imm_float model_trans_get_lprob(struct model_trans const* mtrans) { return mtrans->lprob; }

struct imm_state const* model_trans_get_state(struct model_trans const* mtrans) { return mtrans->state; }

void model_trans_set_lprob(struct model_trans* mtrans, imm_float lprob) { mtrans->lprob = lprob; }

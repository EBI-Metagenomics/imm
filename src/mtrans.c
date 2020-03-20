#include "mtrans.h"
#include "free.h"
#include "imm/bug.h"

struct mtrans
{
    struct imm_state const* state;
    double                  lprob;
};

struct mtrans* mtrans_create(struct imm_state const* state, double const lprob)
{
    struct mtrans* mtrans = malloc(sizeof(struct mtrans));
    mtrans->state = state;
    mtrans->lprob = lprob;
    return mtrans;
}

void mtrans_destroy(struct mtrans const* mtrans)
{
    IMM_BUG(mtrans == NULL);
    imm_free(mtrans);
}

struct imm_state const* mtrans_get_state(struct mtrans const* mtrans) { return mtrans->state; }

double mtrans_get_lprob(struct mtrans const* mtrans) { return mtrans->lprob; }

void mtrans_set_lprob(struct mtrans* mtrans, double lprob) { mtrans->lprob = lprob; }

#include "mtrans.h"
#include "free.h"
#include "imm/bug.h"

struct mtrans
{
    struct imm_state const* state;
    double                  lprob;
};

struct mtrans* mtrans_create(struct imm_state const* state, double lprob)
{
    struct mtrans* mtrans = malloc(sizeof(*mtrans));
    mtrans->state = state;
    mtrans->lprob = lprob;
    return mtrans;
}

void mtrans_destroy(struct mtrans const* mtrans)
{
    IMM_BUG(mtrans == NULL);
    free_c(mtrans);
}

double mtrans_get_lprob(struct mtrans const* mtrans) { return mtrans->lprob; }

struct imm_state const* mtrans_get_state(struct mtrans const* mtrans) { return mtrans->state; }

void mtrans_set_lprob(struct mtrans* mtrans, double lprob) { mtrans->lprob = lprob; }

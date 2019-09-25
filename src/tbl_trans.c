#include "tbl_trans.h"
#include "uthash.h"
#include <math.h>

struct tbl_trans
{
    int state_id;
    double lprob;
    UT_hash_handle hh;
};

void tbl_trans_create(struct tbl_trans **tbl_transitions) { *tbl_transitions = NULL; }

void tbl_trans_destroy(struct tbl_trans **tbl_transitions)
{
    struct tbl_trans *tbl_trans, *tmp;
    if (*tbl_transitions) {
        HASH_ITER(hh, *tbl_transitions, tbl_trans, tmp)
        {
            tbl_trans->state_id = -1;
            tbl_trans->lprob = -INFINITY;
            HASH_DEL(*tbl_transitions, tbl_trans);
            free(tbl_trans);
        }
    }
    *tbl_transitions = NULL;
}

void tbl_trans_set_lprob(struct tbl_trans **tbl_transitions, int state_id, double lprob)
{
    struct tbl_trans *tbl_trans = NULL;
    HASH_FIND_INT(*tbl_transitions, &state_id, tbl_trans);
    if (!tbl_trans) {
        tbl_trans = malloc(sizeof(struct tbl_trans));
        tbl_trans->state_id = state_id;
        HASH_ADD_INT(*tbl_transitions, state_id, tbl_trans);
    }
    tbl_trans->lprob = lprob;
}

double tbl_trans_get_lprob(const struct tbl_trans *tbl_transitions, int state_id)
{
    struct tbl_trans *tbl_trans = NULL;
    HASH_FIND_INT(tbl_transitions, &state_id, tbl_trans);
    if (!tbl_trans)
        return -INFINITY;
    return tbl_trans->lprob;
}

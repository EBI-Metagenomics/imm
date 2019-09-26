#include "tbl_trans.h"
#include "uthash.h"
#include <math.h>

struct tbl_trans
{
    int state_id;
    double lprob;
    UT_hash_handle hh;
};

void tbl_trans_create(struct tbl_trans **head_ptr) { *head_ptr = NULL; }

void tbl_trans_destroy(struct tbl_trans **head_ptr)
{
    struct tbl_trans *tbl_trans, *tmp;
    if (*head_ptr) {
        HASH_ITER(hh, *head_ptr, tbl_trans, tmp)
        {
            HASH_DEL(*head_ptr, tbl_trans);
            free(tbl_trans);
        }
    }
    *head_ptr = NULL;
}

struct tbl_trans *tbl_trans_add(struct tbl_trans **head_ptr, int state_id, double lprob)
{
    struct tbl_trans *tbl_trans = malloc(sizeof(struct tbl_trans));
    tbl_trans->state_id = state_id;
    tbl_trans->lprob = lprob;
    HASH_ADD_INT(*head_ptr, state_id, tbl_trans);
    return tbl_trans;
}

struct tbl_trans *tbl_trans_find(struct tbl_trans *head, int state_id)
{
    struct tbl_trans *tbl_trans = NULL;
    HASH_FIND_INT(head, &state_id, tbl_trans);
    return tbl_trans;
}

const struct tbl_trans *tbl_trans_find_c(const struct tbl_trans *head, int state_id)
{
    const struct tbl_trans *tbl_trans = NULL;
    HASH_FIND_INT(head, &state_id, tbl_trans);
    return tbl_trans;
}

void tbl_trans_set_lprob(struct tbl_trans *tbl_trans, double lprob)
{
    tbl_trans->lprob = lprob;
}

double tbl_trans_get_lprob(const struct tbl_trans *tbl_trans)
{
    return tbl_trans->lprob;
}

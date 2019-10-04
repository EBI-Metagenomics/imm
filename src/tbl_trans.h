#ifndef IMM_TBL_TRANS_H
#define IMM_TBL_TRANS_H

struct tbl_trans;

void tbl_trans_create(struct tbl_trans **head_ptr);
void tbl_trans_destroy(struct tbl_trans **head_ptr);

struct tbl_trans *tbl_trans_add(struct tbl_trans **head_ptr, int state_id,
                                double lprob);

struct tbl_trans *tbl_trans_find(struct tbl_trans *head, int state_id);
const struct tbl_trans *tbl_trans_find_c(const struct tbl_trans *head, int state_id);

void tbl_trans_set_lprob(struct tbl_trans *tbl_trans, double lprob);
double tbl_trans_get_lprob(const struct tbl_trans *tbl_trans);

struct tbl_trans *tbl_trans_next(struct tbl_trans *tbl_trans);
const struct tbl_trans *tbl_trans_next_c(const struct tbl_trans *tbl_trans);

int tbl_trans_get_id(const struct tbl_trans *tbl_trans);

#endif


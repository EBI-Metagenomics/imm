#ifndef IMM_TBL_TRANS_H
#define IMM_TBL_TRANS_H

#include "src/imm/hide.h"

struct tbl_trans;

HIDE void tbl_trans_create(struct tbl_trans **head_ptr);
HIDE void tbl_trans_destroy(struct tbl_trans **head_ptr);

HIDE struct tbl_trans *tbl_trans_add(struct tbl_trans **head_ptr, int state_id, double lprob);

HIDE struct tbl_trans *tbl_trans_find(struct tbl_trans *head, int state_id);
HIDE const struct tbl_trans *tbl_trans_find_c(const struct tbl_trans *head, int state_id);

HIDE void tbl_trans_set_lprob(struct tbl_trans *tbl_trans, double lprob);
HIDE double tbl_trans_get_lprob(const struct tbl_trans *tbl_trans);

HIDE struct tbl_trans *tbl_trans_next(struct tbl_trans *tbl_trans);
HIDE const struct tbl_trans *tbl_trans_next_c(const struct tbl_trans *tbl_trans);

HIDE int tbl_trans_get_id(const struct tbl_trans *tbl_trans);

#endif

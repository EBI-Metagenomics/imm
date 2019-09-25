#ifndef NHMM_TBL_TRANS_H
#define NHMM_TBL_TRANS_H

struct tbl_trans;

void tbl_trans_create(struct tbl_trans **tbl_transitions);
void tbl_trans_set_lprob(struct tbl_trans **tbl_transitions, int state_id,
                         double lprob);
double tbl_trans_get_lprob(const struct tbl_trans *tbl_transitions, int state_id);
void tbl_trans_destroy(struct tbl_trans **tbl_transitions);

#endif


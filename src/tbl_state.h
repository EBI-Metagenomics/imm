#ifndef IMM_TBL_STATE_H
#define IMM_TBL_STATE_H

struct tbl_state;
struct imm_state;

void tbl_state_create(struct tbl_state **head_ptr);
void tbl_state_destroy(struct tbl_state **head_ptr);

void tbl_state_add_state(struct tbl_state **head_ptr, int state_id,
                         const struct imm_state *state, double start_lprob);
int tbl_state_del_state(struct tbl_state **head_ptr, int state_id);

struct tbl_state *tbl_state_find(struct tbl_state *head, int state_id);
const struct tbl_state *tbl_state_find_c(const struct tbl_state *head, int state_id);

const struct imm_state *tbl_state_get_state(const struct tbl_state *tbl_state);
double tbl_state_get_start_lprob(const struct tbl_state *tbl_state);
void tbl_state_set_start_lprob(struct tbl_state *tbl_state, double lprob);

struct tbl_state *tbl_state_next(struct tbl_state *tbl_state);
const struct tbl_state *tbl_state_next_c(const struct tbl_state *tbl_state);

struct tbl_trans *tbl_state_get_trans(struct tbl_state *tbl_state);
const struct tbl_trans *tbl_state_get_trans_c(const struct tbl_state *tbl_state);

struct tbl_trans **tbl_state_get_trans_ptr(struct tbl_state *tbl_state);
struct tbl_trans *const *tbl_state_get_trans_ptr_c(const struct tbl_state *tbl_state);

#endif

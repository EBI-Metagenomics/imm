#ifndef NHMM_TBL_STATE_H
#define NHMM_TBL_STATE_H

struct tbl_trans;

void tbl_trans_create(struct tbl_trans **tbl_transitions);
void tbl_trans_set_lprob(struct tbl_trans **tbl_transitions, int state_id,
                         double lprob);
double tbl_trans_get_lprob(const struct tbl_trans *tbl_transitions, int state_id);
void tbl_trans_destroy(struct tbl_trans **tbl_transitions);

struct tbl_state;
struct nhmm_state;

void tbl_state_create(struct tbl_state **tbl_states);
void tbl_state_add_state(struct tbl_state **tbl_states, int state_id,
                         const struct nhmm_state *state, double start_lprob);
int tbl_state_del_state(struct tbl_state **tbl_states, int state_id);
const struct tbl_state *tbl_state_find_c(const struct tbl_state *tbl_states,
                                         int state_id);
struct tbl_state *tbl_state_find(struct tbl_state *tbl_states, int state_id);
const struct nhmm_state *tbl_state_get_state(const struct tbl_state *tbl_state);
double tbl_state_get_start_lprob(const struct tbl_state *tbl_state);
void tbl_state_set_start_lprob(struct tbl_state *tbl_state, double lprob);
struct tbl_trans **tbl_state_get_transitions(struct tbl_state *tbl_states,
                                             int state_id);
const struct tbl_state *tbl_state_next_c(const struct tbl_state *tbl_state);
struct tbl_state *tbl_state_next(struct tbl_state *tbl_state);
void tbl_state_destroy(struct tbl_state **tbl_states);

#endif

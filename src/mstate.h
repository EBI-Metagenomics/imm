#ifndef IMM_MSTATE_H
#define IMM_MSTATE_H

struct imm_state;
struct mtrans_table;

struct mstate
{
    struct imm_state const* state;
    double                  start_lprob;
    struct mtrans_table*    mtrans_table;
};

struct mstate* mstate_create(struct imm_state const* state, double start_lprob);
void           mstate_destroy(struct mstate* mstate);
static inline struct imm_state const* mstate_get_state(struct mstate const* mstate)
{
    return mstate->state;
}
double               mstate_get_start(struct mstate const* mstate);
void                 mstate_set_start(struct mstate* mstate, double lprob);
struct mtrans_table* mstate_get_mtrans_table(struct mstate const* mstate);

#endif

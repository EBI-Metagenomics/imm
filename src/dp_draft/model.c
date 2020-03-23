#include "dp/model.h"
#include "imm/seq_code.h"

/* struct transition */
/* { */
/*     double*   cost; */
/*     unsigned* offset; */
/* }; */

struct emission
{
    double*   cost;
    unsigned* offset;
};

struct dp_model
{
    unsigned nstates;
    double*  start_lprob;
    /* struct transition*      transition; */
    struct emission*        emission;
    unsigned* min_seq;
    unsigned* max_seq;
    struct imm_state const* state;
    /* struct seq_code const*  seq_code; */
};

void emission_create(struct seq_code const* seq_code, unsigned* min_seq, unsigned* max_seq)
{


}

double emission_cost(struct emission const* emission, unsigned state, unsigned seq_code)
{
    return emission->cost[emission->offset[state] + seq_code];
}

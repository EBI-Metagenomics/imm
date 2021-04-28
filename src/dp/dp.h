#ifndef DP_H
#define DP_H

#include "dp/args.h"

/* struct imm_state; */
struct imm_dp;

struct imm_dp *dp_new(struct dp_args const *args);
/* struct imm_dp *dp_create_from(struct imm_state **states, */
/*                               struct seq_code const *seq_code, */
/*                               struct dp_emission const *emission, */
/*                               struct dp_trans_table *trans_table, */
/*                               struct dp_state_table const *state_table); */

#endif

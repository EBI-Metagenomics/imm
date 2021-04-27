#ifndef DP_H
#define DP_H

#include "dp_args.h"
#include <stdio.h>

struct dp_emission;
struct dp_state_table;
struct dp_trans_table;
/* struct imm_hmm; */
/* struct imm_model; */
struct imm_state;

struct imm_dp
{
    struct seq_code const *seq_code;
    struct dp_emission const *emission;
    struct dp_trans_table *trans_table;
    struct dp_state_table const *state_table;
};

struct imm_dp *dp_new(struct dp_args const *args);
/* struct imm_dp *dp_create_from(struct imm_state **states, */
/*                               struct seq_code const *seq_code, */
/*                               struct dp_emission const *emission, */
/*                               struct dp_trans_table *trans_table, */
/*                               struct dp_state_table const *state_table); */
/* struct dp_emission const *dp_get_emission(struct imm_dp const *dp); */
/* struct imm_state **dp_get_states(struct imm_dp const *dp); */
/* struct seq_code const *dp_get_seq_code(struct imm_dp const *dp); */
/* struct dp_state_table const *dp_get_state_table(struct imm_dp const *dp); */
/* struct dp_trans_table *dp_get_trans_table(struct imm_dp const *dp); */

#endif

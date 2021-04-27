#ifndef DP_EMISSION_H
#define DP_EMISSION_H

#include "imm/imm.h"
#include <stdint.h>
/* #include <stdio.h> */

struct seq_code;

struct emission
{
    imm_float *score; /**< Sequence emission score of a state. */
    uint32_t *offset; /**< Maps state to score array offset. */
};

struct emission const *emission_create(struct seq_code const *seq_code,
                                       struct imm_state **states,
                                       unsigned nstates);
void emission_destroy(struct emission const *emission);
/* struct emission const *dp_emission_read(FILE *stream); */
static inline imm_float emission_score(struct emission const *emission,
                                       unsigned state, unsigned seq_code);
/* int dp_emission_write(struct emission const *emission, unsigned nstates,
 */
/*                       FILE *stream); */

static inline imm_float emission_score(struct emission const *emission,
                                       unsigned state, unsigned seq_code)
{
    return emission->score[emission->offset[state] + seq_code];
}

#endif

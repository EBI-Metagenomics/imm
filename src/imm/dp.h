#ifndef IMM_DP_H
#define IMM_DP_H

#include <stddef.h>

struct tbl_state;

struct dp *dp_create(const struct tbl_state *tbl_states, const char *seq,
                     int seq_len);
double dp_viterbi(struct dp *dp);
void dp_destroy(struct dp *dp);

#endif

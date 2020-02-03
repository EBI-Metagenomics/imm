#ifndef IMM_RESULTS_H
#define IMM_RESULTS_H

#include "imm/api.h"
#include "imm/subseq.h"
#include <stdbool.h>

struct imm_seq;
struct imm_results;
struct imm_path;


IMM_API struct imm_results*      imm_results_create(struct imm_seq const* seq,
                                                    unsigned              window_length);
IMM_API void                     imm_results_set(struct imm_results* results, unsigned idx,
                                                 struct imm_viterbi_item item);
IMM_API struct imm_viterbi_item* imm_results_get(struct imm_results* results, unsigned idx);
IMM_API void                     imm_results_destroy(struct imm_results const* results);

#endif

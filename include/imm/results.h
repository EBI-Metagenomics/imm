#ifndef IMM_RESULTS_H
#define IMM_RESULTS_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/subseq.h"

struct imm_path;
struct imm_result;
struct imm_results;
struct imm_seq;

IMM_API void                   imm_result_destroy(struct imm_result const* result);
IMM_API void                   imm_result_free(struct imm_result const* result);
IMM_API struct imm_path const* imm_result_path(struct imm_result const* result);
IMM_API imm_float              imm_result_seconds(struct imm_result const* result);
IMM_API struct imm_subseq      imm_result_subseq(struct imm_result const* result);

IMM_API struct imm_results*      imm_results_create(struct imm_seq const* seq, unsigned nresults);
IMM_API void                     imm_results_destroy(struct imm_results const* results);
IMM_API void                     imm_results_free(struct imm_results const* results);
IMM_API struct imm_result const* imm_results_get(struct imm_results const* results, unsigned idx);
IMM_API void imm_results_set(struct imm_results* results, unsigned idx, struct imm_subseq subseq,
                             struct imm_path const* path, imm_float seconds);
IMM_API unsigned imm_results_size(struct imm_results const* results);

#endif

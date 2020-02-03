#if 0
#include "imm/results.h"
#include <stdlib.h>

struct imm_result
{
    struct imm_subseq      subseq;
    struct imm_path const* path;
    double                 log_likelihood;
};

struct imm_results
{
    struct imm_seq const *seq;
    struct imm_result *result;
    unsigned nresults;
};

struct imm_results*      imm_results_create(struct imm_seq const* seq,
                                                    unsigned              window_length)
{
    struct imm_results *results = malloc(sizeof(struct imm_results));

    results->seq = seq;
    results->result = malloc(sizeof(struct imm_result) * imm_window_size());

    return results;
}
#endif

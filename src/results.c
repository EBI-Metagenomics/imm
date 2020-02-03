#include "imm/results.h"
#include "free.h"
#include "imm/path.h"
#include "imm/subseq.h"
#include "imm/window.h"
#include <stdlib.h>

struct imm_result
{
    struct imm_path const* path;
    struct imm_subseq      subseq;
    double                 loglik;
};

struct imm_results
{
    struct imm_seq const* seq;
    struct imm_window*    window;
    struct imm_result*    result;
    unsigned              nresults;
};

struct imm_results* imm_results_create(struct imm_seq const* seq, unsigned window_length)
{
    struct imm_results* results = malloc(sizeof(struct imm_results));

    results->seq = seq;
    results->window = imm_window_create(seq, window_length);
    results->nresults = imm_window_size(results->window);
    results->result = malloc(sizeof(struct imm_result) * results->nresults);

    return results;
}

void imm_results_set(struct imm_results* results, unsigned idx, struct imm_subseq subseq,
                     struct imm_path const* path, double loglik)
{
    results->result[idx].path = path;
    results->result[idx].subseq = subseq;
    results->result[idx].loglik = loglik;
}

struct imm_result const* imm_results_get(struct imm_results const* results, unsigned idx)
{
    return results->result + idx;
}

unsigned imm_results_size(struct imm_results const* results) { return results->nresults; }

void imm_results_destroy(struct imm_results const* results)
{
    for (unsigned i = 0; i < results->nresults; ++i)
        imm_path_destroy(results->result[i].path);
    free_c(results->result);
    imm_window_destroy(results->window);
    free_c(results);
}

double imm_result_loglik(struct imm_result const* result) { return result->loglik; }

struct imm_path const* imm_result_path(struct imm_result const* result)
{
    return result->path;
}

struct imm_seq const* imm_result_sequence(struct imm_result const* result)
{
    return imm_subseq_cast(&result->subseq);
}

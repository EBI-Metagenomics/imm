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
    unsigned              nresults;
    struct imm_result*    result[];
};

struct imm_results* imm_results_create(struct imm_seq const* seq, unsigned const nresults)
{
    struct imm_results* results =
        malloc(sizeof(struct imm_results) + sizeof(struct imm_result* [nresults]));

    results->seq = seq;
    results->nresults = nresults;
    for (unsigned i = 0; i < results->nresults; ++i)
        results->result[i] = malloc(sizeof(struct imm_result));

    return results;
}

void imm_results_set(struct imm_results* results, unsigned idx, struct imm_subseq subseq,
                     struct imm_path const* path, double loglik)
{
    results->result[idx]->path = path;
    results->result[idx]->subseq = subseq;
    results->result[idx]->loglik = loglik;
}

struct imm_result const* imm_results_get(struct imm_results const* results, unsigned idx)
{
    return results->result[idx];
}

unsigned imm_results_size(struct imm_results const* results) { return results->nresults; }

void imm_results_destroy(struct imm_results const* results)
{
    for (unsigned i = 0; i < results->nresults; ++i)
        imm_result_destroy(results->result[i]);

    imm_results_free(results);
}

void imm_results_free(struct imm_results const* results) { free_c(results); }

double imm_result_loglik(struct imm_result const* result) { return result->loglik; }

struct imm_path const* imm_result_path(struct imm_result const* result)
{
    return result->path;
}

struct imm_subseq imm_result_subseq(struct imm_result const* result) { return result->subseq; }

void imm_result_destroy(struct imm_result const* result)
{
    imm_path_destroy(result->path);
    imm_result_free(result);
}

void imm_result_free(struct imm_result const* result) { free_c(result); }

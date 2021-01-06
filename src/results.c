#include "imm/results.h"
#include "free.h"
#include "imm/path.h"
#include "imm/subseq.h"
#include <stdlib.h>

struct imm_result
{
    struct imm_path const* path;
    struct imm_subseq      subseq;
    double                 loglik;
    double                 seconds;
};

struct imm_results
{
    struct imm_seq const* seq;
    unsigned              nresults;
    struct imm_result*    result[];
};

void imm_result_destroy(struct imm_result const* result)
{
    imm_path_destroy(result->path);
    free_c(result);
}

void imm_result_free(struct imm_result const* result) { free_c(result); }

double imm_result_loglik(struct imm_result const* result) { return result->loglik; }

struct imm_path const* imm_result_path(struct imm_result const* result) { return result->path; }

double imm_result_seconds(struct imm_result const* result) { return result->seconds; }

struct imm_subseq imm_result_subseq(struct imm_result const* result) { return result->subseq; }

struct imm_results* imm_results_create(struct imm_seq const* seq, unsigned const nresults)
{
    struct imm_results* results = malloc(sizeof(*results) + sizeof(struct imm_result* [nresults]));

    results->seq = seq;
    results->nresults = nresults;
    for (unsigned i = 0; i < results->nresults; ++i)
        results->result[i] = malloc(sizeof(*results->result[i]));

    return results;
}

void imm_results_destroy(struct imm_results const* results)
{
    for (unsigned i = 0; i < results->nresults; ++i)
        imm_result_destroy(results->result[i]);

    free_c(results);
}

void imm_results_free(struct imm_results const* results) { free_c(results); }

struct imm_result const* imm_results_get(struct imm_results const* results, unsigned idx)
{
    return results->result[idx];
}

void imm_results_set(struct imm_results* results, unsigned idx, struct imm_subseq subseq,
                     struct imm_path const* path, double loglik, double seconds)
{
    results->result[idx]->path = path;
    results->result[idx]->subseq = subseq;
    results->result[idx]->loglik = loglik;
    results->result[idx]->seconds = seconds;
}

unsigned imm_results_size(struct imm_results const* results) { return results->nresults; }

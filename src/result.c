#include "result.h"
#include "free.h"
#include <stdlib.h>

struct imm_result
{
    struct imm_seq const*  seq;
    struct imm_path const* path;
    imm_float              seconds;
};

struct imm_result* imm_result_create(struct imm_seq const* seq)
{
    struct imm_result* result = malloc(sizeof(*result));
    result->seq = seq;
    result->path = NULL;
    result->seconds = 0.;
    return result;
}

void imm_result_destroy(struct imm_result const* result)
{
    imm_path_destroy(result->path);
    free_c(result);
}

void imm_result_free(struct imm_result const* result) { free_c(result); }

struct imm_path const* imm_result_path(struct imm_result const* result) { return result->path; }

imm_float imm_result_seconds(struct imm_result const* result) { return result->seconds; }

void result_set(struct imm_result* result, struct imm_path const* path, imm_float seconds)
{
    result->path = path;
    result->seconds = seconds;
}

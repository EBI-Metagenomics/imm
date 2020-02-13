#include "imm/abct.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/lprob.h"
#include "imm/report.h"
#include <stdlib.h>
#include <string.h>

struct imm_abct
{
    struct imm_abc const* abc;
    double                lprobs[];
};

struct imm_abct const* imm_abct_create(struct imm_abc const* abc, double const* lprobs)
{
    struct imm_abct* abct =
        malloc(sizeof(struct imm_abct) + sizeof(double) * imm_abc_length(abc));
    abct->abc = abc;
    memcpy(abct->lprobs, lprobs, sizeof(double) * imm_abc_length(abc));
    return abct;
}

double imm_abct_lprob(struct imm_abct const* abct, char const symbol)
{
    int idx = imm_abc_symbol_idx(abct->abc, symbol);
    if (idx < 0) {
        imm_error("symbol not found");
        return imm_lprob_invalid();
    }
    size_t i = (size_t)idx;
    return abct->lprobs[i];
}

void imm_abct_destroy(struct imm_abct const* abct) { free_c(abct); }

struct imm_abc const* imm_abct_get_abc(struct imm_abct const* abct) { return abct->abc; }

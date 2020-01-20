#include "free.h"
#include "imm/imm.h"
#include "min.h"
#include "uthash.h"

struct emission;

struct imm_sequence_table
{
    struct imm_abc const* abc;
    struct emission*      emissions;
    int                   min_seq;
    int                   max_seq;
};

struct emission
{
    char const*    seq;
    double         lprob;
    UT_hash_handle hh;
};

struct imm_sequence_table* imm_sequence_table_create(struct imm_abc const* abc)
{
    struct imm_sequence_table* table = malloc(sizeof(struct imm_sequence_table));
    table->abc = abc;
    table->emissions = NULL;
    table->min_seq = 0;
    table->max_seq = 0;
    return table;
}

void imm_sequence_table_destroy(struct imm_sequence_table const* table)
{
    if (!table) {
        imm_error("table should not be NULL");
        return;
    }

    struct emission* emiss = NULL;
    struct emission* tmp = NULL;
    HASH_ITER(hh, table->emissions, emiss, tmp)
    {
        free_c(emiss->seq);
        /* TODO: this hash library shouldn't be changing emission pointer */
        struct imm_sequence_table* t = (struct imm_sequence_table*)table;
        HASH_DEL(t->emissions, emiss);
        free_c(emiss);
    }

    free_c(table);
}

int imm_sequence_table_add(struct imm_sequence_table* table, char const* seq,
                           double const lprob)
{
    int const seq_len = (int const)strlen(seq);

    for (int i = 0; i < seq_len; ++i) {
        if (!imm_abc_has_symbol(table->abc, seq[i])) {
            imm_error("symbol not found in the alphabet");
            return 1;
        }
    }

    struct emission* emiss = malloc(sizeof(struct emission));
    emiss->seq = strdup(seq);
    emiss->lprob = lprob;
    HASH_ADD_STR(table->emissions, seq, emiss);
    table->min_seq = MIN(table->min_seq, seq_len);
    table->max_seq = MAX(table->max_seq, seq_len);

    return 0;
}

int imm_sequence_table_normalize(struct imm_sequence_table* table)
{
    int const len = (int)HASH_CNT(hh, table->emissions);
    double*   lprobs = malloc(sizeof(double) * (size_t)len);

    struct emission* emiss = NULL;
    struct emission* tmp = NULL;
    size_t           i = 0;
    HASH_ITER(hh, table->emissions, emiss, tmp)
    {
        lprobs[i] = emiss->lprob;
        ++i;
    }
    if (imm_lprob_normalize(lprobs, len)) {
        free_c(lprobs);
        return 1;
    }

    i = 0;
    HASH_ITER(hh, table->emissions, emiss, tmp)
    {
        emiss->lprob = lprobs[i];
        ++i;
    }

    free_c(lprobs);
    return 0;
}

double imm_sequence_table_lprob(struct imm_sequence_table const* table, char const* seq,
                                int seq_len)
{
    struct emission* emiss = NULL;
    HASH_FIND(hh, table->emissions, seq, (size_t)seq_len, emiss);

    if (emiss)
        return emiss->lprob;

    return imm_lprob_zero();
}

struct imm_abc const* imm_sequence_table_get_abc(struct imm_sequence_table const* table)
{
    return table->abc;
}

int imm_sequence_table_min_seq(struct imm_sequence_table const* table)
{
    return table->min_seq;
}

int imm_sequence_table_max_seq(struct imm_sequence_table const* table)
{
    return table->max_seq;
}

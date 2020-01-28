#include "free.h"
#include "imm/imm.h"
#include "khash_seq.h"
#include "min.h"

struct emission
{
    struct imm_seq seq;
    double         lprob;
};

KHASH_MAP_INIT_SEQ(emission, struct emission*)

struct imm_seq_table
{
    struct imm_abc const* abc;
    khash_t(emission) * emission_table;
    unsigned min_seq;
    unsigned max_seq;
};

struct imm_seq_table* imm_seq_table_create(struct imm_abc const* abc)
{
    struct imm_seq_table* table = malloc(sizeof(struct imm_seq_table));
    table->abc = abc;
    table->min_seq = 0;
    table->max_seq = 0;
    table->emission_table = kh_init(emission);
    return table;
}

void imm_seq_table_destroy(struct imm_seq_table const* table)
{
    if (!table) {
        imm_error("table should not be NULL");
        return;
    }

    khash_t(emission)* emiss_tbl = table->emission_table;

    for (khiter_t i = kh_begin(emiss_tbl); i < kh_end(emiss_tbl); ++i) {
        if (kh_exist(emiss_tbl, i)) {
            struct emission const* e = kh_val(emiss_tbl, i);
            imm_seq_destroy(e->seq);
            free_c(e);
        }
    }

    kh_destroy(emission, emiss_tbl);
    free_c(table);
}

int imm_seq_table_add(struct imm_seq_table* table, struct imm_seq const seq,
                      double const lprob)
{
    for (unsigned i = 0; i < seq.length; ++i) {
        if (!imm_abc_has_symbol(table->abc, seq.string[i])) {
            imm_error("symbol not found in the alphabet");
            return 1;
        }
    }

    int      ret = 0;
    khiter_t iter = kh_put(emission, table->emission_table, seq, &ret);
    if (ret == -1) {
        imm_error("hash table failed");
        return 1;
    }
    if (ret == 0) {
        imm_error("sequence already exist");
        return 1;
    }

    struct emission* emiss = malloc(sizeof(struct emission));
    emiss->seq = imm_seq_duplicate(seq);
    emiss->lprob = lprob;

    kh_key(table->emission_table, iter) = emiss->seq;
    kh_val(table->emission_table, iter) = emiss;

    table->min_seq = MIN(table->min_seq, seq.length);
    table->max_seq = MAX(table->max_seq, seq.length);

    return 0;
}

int imm_seq_table_normalize(struct imm_seq_table* table)
{
    khiter_t const len = kh_size(table->emission_table);
    double*        lprobs = malloc(sizeof(double) * (size_t)len);
    double*        lprob = lprobs;

    khash_t(emission)* emiss_tbl = table->emission_table;

    for (khiter_t i = kh_begin(emiss_tbl); i < kh_end(emiss_tbl); ++i) {
        if (kh_exist(emiss_tbl, i)) {
            *lprob = kh_val(emiss_tbl, i)->lprob;
            ++lprob;
        }
    }

    if (imm_lprob_normalize(lprobs, len)) {
        free_c(lprobs);
        return 1;
    }

    lprob = lprobs;
    for (khiter_t i = kh_begin(emiss_tbl); i < kh_end(emiss_tbl); ++i) {
        if (kh_exist(emiss_tbl, i)) {
            kh_val(emiss_tbl, i)->lprob = *lprob;
            ++lprob;
        }
    }

    free_c(lprobs);
    return 0;
}

double imm_seq_table_lprob(struct imm_seq_table const* table, struct imm_seq const seq)
{
    khiter_t i = kh_get(emission, table->emission_table, seq);

    if (i == kh_end(table->emission_table))
        return imm_lprob_zero();

    return kh_val(table->emission_table, i)->lprob;
}

struct imm_abc const* imm_seq_table_get_abc(struct imm_seq_table const* table)
{
    return table->abc;
}

unsigned imm_seq_table_min_seq(struct imm_seq_table const* table) { return table->min_seq; }

unsigned imm_seq_table_max_seq(struct imm_seq_table const* table) { return table->max_seq; }

#include "imm/seq_table.h"
#include "cast.h"
#include "free.h"
#include "imm//bug.h"
#include "imm/lprob.h"
#include "imm/report.h"
#include "khash_seq.h"
#include "min.h"

struct emission
{
    struct imm_seq const* seq;
    double                lprob;
};

KHASH_MAP_INIT_SEQ(emission, struct emission*)

struct imm_seq_table
{
    struct imm_abc const* abc;
    khash_t(emission) * emission_table;
    uint8_t min_seq;
    uint8_t max_seq;
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

struct imm_seq_table* imm_seq_table_clone(struct imm_seq_table const* table)
{
    struct imm_seq_table* new_table = malloc(sizeof(struct imm_seq_table));
    new_table->abc = table->abc;
    new_table->min_seq = table->min_seq;
    new_table->max_seq = table->max_seq;
    new_table->emission_table = kh_init(emission);

    khash_t(emission)* emiss_tbl = table->emission_table;

    for (khiter_t i = kh_begin(emiss_tbl); i < kh_end(emiss_tbl); ++i) {
        if (kh_exist(emiss_tbl, i)) {
            struct emission const* e = kh_val(emiss_tbl, i);
            IMM_BUG(imm_seq_table_add(new_table, e->seq, e->lprob) != 0);
        }
    }
    return new_table;
}

void imm_seq_table_destroy(struct imm_seq_table const* table)
{
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

int imm_seq_table_add(struct imm_seq_table* table, struct imm_seq const* seq, double const lprob)
{
    if (table->abc != imm_seq_get_abc(seq)) {
        imm_error("alphabets must be the same");
        return 1;
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
    emiss->seq = imm_seq_clone(seq);
    emiss->lprob = lprob;

    kh_key(table->emission_table, iter) = emiss->seq;
    kh_val(table->emission_table, iter) = emiss;

    table->min_seq = MIN(table->min_seq, cast_u8_u(imm_seq_length(seq)));
    table->max_seq = MAX(table->max_seq, cast_u8_u(imm_seq_length(seq)));

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

double imm_seq_table_lprob(struct imm_seq_table const* table, struct imm_seq const* seq)
{
    if (table->abc != imm_seq_get_abc(seq)) {
        imm_error("alphabets must be the same");
        return imm_lprob_invalid();
    }

    khiter_t i = kh_get(emission, table->emission_table, seq);

    if (i == kh_end(table->emission_table))
        return imm_lprob_zero();

    return kh_val(table->emission_table, i)->lprob;
}

struct imm_abc const* imm_seq_table_get_abc(struct imm_seq_table const* table)
{
    return table->abc;
}

uint8_t imm_seq_table_min_seq(struct imm_seq_table const* table) { return table->min_seq; }

uint8_t imm_seq_table_max_seq(struct imm_seq_table const* table) { return table->max_seq; }

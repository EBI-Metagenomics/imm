#ifndef MODEL_H
#define MODEL_H

#include <stdint.h>
#include <stdio.h>

struct dp_emission;
struct dp_state_table;
struct imm_hmm;
struct imm_profile;
struct seq_code;

struct imm_model
{
    struct imm_hmm*              hmm;
    struct imm_dp*               dp;
    struct seq_code const*       seq_code;
    struct dp_emission const*    emission;
    struct dp_trans_table*       trans_table;
    struct dp_state_table const* state_table;
};

void                    model_deep_destroy(struct imm_model const* model);
struct imm_model const* model_read(struct imm_profile* prof, FILE* stream);
struct imm_state*       model_read_state(struct imm_profile* prof, FILE* stream);
int                     model_write(struct imm_profile const* prof, struct imm_model const* model, FILE* stream);
int                     model_write_state(struct imm_profile const* prof, FILE* stream, struct imm_state const* state);

#endif

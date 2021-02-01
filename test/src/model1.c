#include "model1.h"
#include "helper.h"

static inline struct imm_state const* normal_super(struct imm_normal_state const* state)
{
    return imm_normal_state_super(state);
}
static inline struct imm_state const* mute_super(struct imm_mute_state const* state)
{
    return imm_mute_state_super(state);
}

struct model1 create_model1(void)
{
    int           ncore_nodes = 1000;
    struct model1 m = {.hmm = NULL,
                       .abc = NULL,
                       .mute_states = imm_vecp_create(),
                       .normal_states = imm_vecp_create()};

    struct imm_abc const* abc = imm_abc_create("BMIEJ", '*');
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    m.abc = abc;
    m.hmm = hmm;

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);
    imm_hmm_add_state(hmm, mute_super(start), logf(1.0f));
    imm_vecp_append(m.mute_states, start);

    imm_float end_lprobs[] = {logf(0.05f), logf(0.05f), logf(0.05f), logf(0.05f), logf(0.05f)};
    struct imm_normal_state const* end = imm_normal_state_create("END", abc, end_lprobs);
    imm_hmm_add_state(hmm, normal_super(end), zero());
    m.end = end;

    imm_float B_lprobs[] = {logf(1.0f), zero(), zero(), zero(), zero()};
    imm_float E_lprobs[] = {zero(), zero(), zero(), logf(1.0f), zero()};
    imm_float J_lprobs[] = {zero(), zero(), zero(), zero(), logf(1.0f)};
    imm_float M_lprobs[] = {zero(), logf(1.0f), zero(), zero(), zero()};
    imm_float I_lprobs[] = {zero(), zero(), logf(1.0f), zero(), zero()};

    struct imm_normal_state const* B = imm_normal_state_create("B", abc, B_lprobs);
    imm_hmm_add_state(hmm, normal_super(B), zero());
    imm_vecp_append(m.normal_states, B);
    struct imm_normal_state const* E = imm_normal_state_create("E", abc, E_lprobs);
    imm_hmm_add_state(hmm, normal_super(E), zero());
    imm_vecp_append(m.normal_states, E);
    struct imm_normal_state const* J = imm_normal_state_create("J", abc, J_lprobs);
    imm_hmm_add_state(hmm, normal_super(J), zero());
    imm_vecp_append(m.normal_states, J);

    imm_hmm_set_trans(hmm, mute_super(start), normal_super(B), logf(0.2f));
    imm_hmm_set_trans(hmm, normal_super(B), normal_super(B), logf(0.2f));
    imm_hmm_set_trans(hmm, normal_super(E), normal_super(E), logf(0.2f));
    imm_hmm_set_trans(hmm, normal_super(J), normal_super(J), logf(0.2f));
    imm_hmm_set_trans(hmm, normal_super(E), normal_super(J), logf(0.2f));
    imm_hmm_set_trans(hmm, normal_super(J), normal_super(B), logf(0.2f));
    imm_hmm_set_trans(hmm, normal_super(E), normal_super(end), logf(0.2f));
    imm_hmm_set_trans(hmm, normal_super(end), normal_super(end), logf(0.2f));

    struct imm_normal_state const* M[ncore_nodes];
    struct imm_normal_state const* I[ncore_nodes];
    struct imm_mute_state const*   D[ncore_nodes];

    char name[10] = "\0";
    for (int i = 0; i < ncore_nodes; ++i) {
        M[i] = imm_normal_state_create(fmt_name(name, "M", i), abc, M_lprobs);
        I[i] = imm_normal_state_create(fmt_name(name, "I", i), abc, I_lprobs);
        D[i] = imm_mute_state_create(fmt_name(name, "D", i), abc);

        imm_hmm_add_state(hmm, normal_super(M[i]), zero());
        imm_vecp_append(m.normal_states, M[i]);
        imm_hmm_add_state(hmm, normal_super(I[i]), zero());
        imm_vecp_append(m.normal_states, I[i]);
        imm_hmm_add_state(hmm, mute_super(D[i]), zero());
        imm_vecp_append(m.normal_states, D[i]);

        if (i == 0)
            imm_hmm_set_trans(hmm, normal_super(B), normal_super(M[0]), logf(0.2f));

        imm_hmm_set_trans(hmm, normal_super(M[i]), normal_super(I[i]), logf(0.2f));
        imm_hmm_set_trans(hmm, normal_super(I[i]), normal_super(I[i]), logf(0.2f));

        if (i > 0) {
            imm_hmm_set_trans(hmm, normal_super(M[i - 1]), normal_super(M[i]), logf(0.2f));
            imm_hmm_set_trans(hmm, mute_super(D[i - 1]), normal_super(M[i]), logf(0.2f));
            imm_hmm_set_trans(hmm, normal_super(I[i - 1]), normal_super(M[i]), logf(0.2f));

            imm_hmm_set_trans(hmm, normal_super(M[i - 1]), mute_super(D[i]), logf(0.2f));
            imm_hmm_set_trans(hmm, mute_super(D[i - 1]), mute_super(D[i]), logf(0.2f));
        }

        if (i == ncore_nodes - 1) {
            imm_hmm_set_trans(hmm, normal_super(M[i]), normal_super(E), logf(0.2f));
            imm_hmm_set_trans(hmm, mute_super(D[i]), normal_super(E), logf(0.2f));
            imm_hmm_set_trans(hmm, normal_super(I[i]), normal_super(E), logf(0.2f));
        }
    }

    return m;
}

void destroy_model1(struct model1 model)
{
    for (size_t i = 0; i < imm_vecp_length(model.mute_states); ++i) {
        struct imm_mute_state const* s = imm_vecp_get(model.mute_states, i);
        imm_mute_state_destroy(s);
    }

    for (size_t i = 0; i < imm_vecp_length(model.normal_states); ++i) {
        struct imm_normal_state const* s = imm_vecp_get(model.normal_states, i);
        imm_normal_state_destroy(s);
    }

    imm_normal_state_destroy(model.end);
    imm_abc_destroy(model.abc);
    imm_hmm_destroy(model.hmm);
    imm_vecp_destroy(model.mute_states);
    imm_vecp_destroy(model.normal_states);
}

char const* get_model1_str1(void)
{
    static char const str[] = "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "BIIIMIMIMIMMMMMMIMMIMMMMMIIMIMMIMIMIMIMIMMMIMMIMME"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMME";
    return str;
}

char const* get_model1_str2(void)
{
    static char const str[] = "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                              "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                              "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMME";

    return str;
}

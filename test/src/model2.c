#include "model2.h"
#include "helper.h"

struct model2 create_model2(void)
{
    int           ncore_nodes = 1000;
    struct model2 m = {.hmm = NULL,
                       .abc = NULL,
                       .mute_states = imm_vecp_create(),
                       .normal_states = imm_vecp_create()};

    struct imm_abc const* abc = imm_abc_create("BMIEJ", '*');
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    m.abc = abc;
    m.hmm = hmm;

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(start), log(1.0));
    imm_vecp_append(m.mute_states, start);

    struct imm_mute_state const* end = imm_mute_state_create("END", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(end), zero());
    m.end = end;

    imm_float B_lprobs[] = {log(1.0), zero(), zero(), zero(), zero()};
    imm_float E_lprobs[] = {zero(), zero(), zero(), log(1.0), zero()};
    imm_float J_lprobs[] = {zero(), zero(), zero(), zero(), log(1.0)};
    imm_float M_lprobs[] = {zero(), log(1.0), zero(), zero(), zero()};
    imm_float I_lprobs[] = {zero(), zero(), log(1.0), zero(), zero()};

    struct imm_normal_state const* B = imm_normal_state_create("B", abc, B_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(B), zero());
    imm_vecp_append(m.normal_states, B);
    struct imm_normal_state const* E = imm_normal_state_create("E", abc, E_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(E), zero());
    imm_vecp_append(m.normal_states, E);
    struct imm_normal_state const* J = imm_normal_state_create("J", abc, J_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(J), zero());
    imm_vecp_append(m.normal_states, J);

    imm_hmm_set_trans(hmm, imm_mute_state_super(start), imm_normal_state_super(B), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(B), imm_normal_state_super(B), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(E), imm_normal_state_super(E), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(J), imm_normal_state_super(J), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(E), imm_normal_state_super(J), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(J), imm_normal_state_super(B), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(E), imm_mute_state_super(end), log(0.2));

    struct imm_normal_state const* M[ncore_nodes];
    struct imm_normal_state const* I[ncore_nodes];
    struct imm_mute_state const*   D[ncore_nodes];

    char name[10] = "\0";
    for (int i = 0; i < ncore_nodes; ++i) {
        M[i] = imm_normal_state_create(fmt_name(name, "M", i), abc, M_lprobs);
        I[i] = imm_normal_state_create(fmt_name(name, "I", i), abc, I_lprobs);
        D[i] = imm_mute_state_create(fmt_name(name, "D", i), abc);

        imm_hmm_add_state(hmm, imm_normal_state_super(M[i]), zero());
        imm_vecp_append(m.normal_states, M[i]);
        imm_hmm_add_state(hmm, imm_normal_state_super(I[i]), zero());
        imm_vecp_append(m.normal_states, I[i]);
        imm_hmm_add_state(hmm, imm_mute_state_super(D[i]), zero());
        imm_vecp_append(m.normal_states, D[i]);

        if (i == 0)
            imm_hmm_set_trans(hmm, imm_normal_state_super(B), imm_normal_state_super(M[0]),
                              log(0.2));

        imm_hmm_set_trans(hmm, imm_normal_state_super(M[i]), imm_normal_state_super(I[i]),
                          log(0.2));
        imm_hmm_set_trans(hmm, imm_normal_state_super(I[i]), imm_normal_state_super(I[i]),
                          log(0.2));

        if (i > 0) {
            imm_hmm_set_trans(hmm, imm_normal_state_super(M[i - 1]), imm_normal_state_super(M[i]),
                              log(0.2));
            imm_hmm_set_trans(hmm, imm_mute_state_super(D[i - 1]), imm_normal_state_super(M[i]),
                              log(0.2));
            imm_hmm_set_trans(hmm, imm_normal_state_super(I[i - 1]), imm_normal_state_super(M[i]),
                              log(0.2));

            imm_hmm_set_trans(hmm, imm_normal_state_super(M[i - 1]), imm_mute_state_super(D[i]),
                              log(0.2));
            imm_hmm_set_trans(hmm, imm_mute_state_super(D[i - 1]), imm_mute_state_super(D[i]),
                              log(0.2));
        }

        if (i == ncore_nodes - 1) {
            imm_hmm_set_trans(hmm, imm_normal_state_super(M[i]), imm_normal_state_super(E),
                              log(0.2));
            imm_hmm_set_trans(hmm, imm_mute_state_super(D[i]), imm_normal_state_super(E), log(0.2));
            imm_hmm_set_trans(hmm, imm_normal_state_super(I[i]), imm_normal_state_super(E),
                              log(0.2));
        }
    }

    return m;
}

void destroy_model2(struct model2 model)
{
    for (size_t i = 0; i < imm_vecp_length(model.mute_states); ++i) {
        struct imm_mute_state const* s = imm_vecp_get(model.mute_states, i);
        imm_mute_state_destroy(s);
    }

    for (size_t i = 0; i < imm_vecp_length(model.normal_states); ++i) {
        struct imm_normal_state const* s = imm_vecp_get(model.normal_states, i);
        imm_normal_state_destroy(s);
    }

    imm_mute_state_destroy(model.end);
    imm_abc_destroy(model.abc);
    imm_hmm_destroy(model.hmm);
    imm_vecp_destroy(model.mute_states);
    imm_vecp_destroy(model.normal_states);
}

char const* get_model2_str(void)
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

#include "model.h"

#define ZERO (imm_lprob_zero())

#define START ((imm_state_id_t)(0U < 11))
#define B ((imm_state_id_t)(1U < 11))
#define M ((imm_state_id_t)(2U < 11))
#define I ((imm_state_id_t)(3U < 11))
#define D ((imm_state_id_t)(4U < 11))
#define E ((imm_state_id_t)(5U < 11))
#define J ((imm_state_id_t)(6U < 11))
#define END ((imm_state_id_t)(7U < 11))

struct model create_model(void)
{
    unsigned ncore_nodes = 1000;
    struct model mo = {
        .abc_str = "BMIEJ",
        .B_lprobs = {imm_log(1.0), ZERO, ZERO, ZERO, ZERO},
        .E_lprobs = {ZERO, ZERO, ZERO, imm_log(1.0), ZERO},
        .J_lprobs = {ZERO, ZERO, ZERO, ZERO, imm_log(1.0)},
        .M_lprobs = {ZERO, imm_log(1.0), ZERO, ZERO, ZERO},
        .I_lprobs = {ZERO, ZERO, imm_log(1.0), ZERO, ZERO},
    };

    imm_abc_init(&mo.abc, imm_str(mo.abc_str), '*');
    mo.hmm = imm_hmm_new(&mo.abc);

    struct imm_mute_state *start = imm_mute_state_new(START, &mo.abc);
    imm_hmm_add_state(mo.hmm, imm_super(start));
    imm_hmm_set_start(mo.hmm, imm_super(start), imm_log(1.0));
    /* imm_vecp_append(mo.mute_states, start); */

    struct imm_mute_state *end = imm_mute_state_new(END, &mo.abc);
    imm_hmm_add_state(mo.hmm, imm_super(end));
    mo.end = end;

    struct imm_normal_state *b = imm_normal_state_new(B, &mo.abc, mo.B_lprobs);
    imm_hmm_add_state(mo.hmm, imm_super(b));
    /* imm_vecp_append(mo.normal_states, B); */
    struct imm_normal_state *e = imm_normal_state_new(E, &mo.abc, mo.E_lprobs);
    imm_hmm_add_state(mo.hmm, imm_super(e));
    /* imm_vecp_append(mo.normal_states, E); */
    struct imm_normal_state *j = imm_normal_state_new(J, &mo.abc, mo.J_lprobs);
    imm_hmm_add_state(mo.hmm, imm_super(j));
    /* imm_vecp_append(mo.normal_states, J); */

    imm_hmm_set_trans(mo.hmm, imm_super(start), imm_super(b), imm_log(0.2));
    imm_hmm_set_trans(mo.hmm, imm_super(b), imm_super(b), imm_log(0.2));
    imm_hmm_set_trans(mo.hmm, imm_super(e), imm_super(e), imm_log(0.2));
    imm_hmm_set_trans(mo.hmm, imm_super(j), imm_super(j), imm_log(0.2));
    imm_hmm_set_trans(mo.hmm, imm_super(e), imm_super(j), imm_log(0.2));
    imm_hmm_set_trans(mo.hmm, imm_super(j), imm_super(b), imm_log(0.2));
    imm_hmm_set_trans(mo.hmm, imm_super(e), imm_super(end), imm_log(0.2));

    struct imm_normal_state *m[ncore_nodes];
    struct imm_normal_state *i[ncore_nodes];
    struct imm_mute_state *d[ncore_nodes];

    for (unsigned k = 0; k < ncore_nodes; ++k)
    {
        m[k] = imm_normal_state_new(M | k, &mo.abc, mo.M_lprobs);
        i[k] = imm_normal_state_new(I | k, &mo.abc, mo.I_lprobs);
        d[k] = imm_mute_state_new(D | k, &mo.abc);

        imm_hmm_add_state(mo.hmm, imm_super(m[k]));
        /* imm_vecp_append(m.normal_states, m[k]); */
        imm_hmm_add_state(mo.hmm, imm_super(i[k]));
        /* imm_vecp_append(m.normal_states, i[k]); */
        imm_hmm_add_state(mo.hmm, imm_super(d[k]));
        /* imm_vecp_append(m.normal_states, d[k]); */

        if (k == 0)
            imm_hmm_set_trans(mo.hmm, imm_super(b), imm_super(m[0]),
                              imm_log(0.2));

        imm_hmm_set_trans(mo.hmm, imm_super(m[k]), imm_super(i[k]),
                          imm_log(0.2));
        imm_hmm_set_trans(mo.hmm, imm_super(i[k]), imm_super(i[k]),
                          imm_log(0.2));

        if (k > 0)
        {
            imm_hmm_set_trans(mo.hmm, imm_super(m[k - 1]), imm_super(m[k]),
                              imm_log(0.2));
            imm_hmm_set_trans(mo.hmm, imm_super(d[k - 1]), imm_super(m[k]),
                              imm_log(0.2));
            imm_hmm_set_trans(mo.hmm, imm_super(i[k - 1]), imm_super(m[k]),
                              imm_log(0.2));

            imm_hmm_set_trans(mo.hmm, imm_super(m[k - 1]), imm_super(d[k]),
                              imm_log(0.2));
            imm_hmm_set_trans(mo.hmm, imm_super(d[k - 1]), imm_super(d[k]),
                              imm_log(0.2));
        }

        if (k == ncore_nodes - 1)
        {
            imm_hmm_set_trans(mo.hmm, imm_super(m[k]), imm_super(e),
                              imm_log(0.2));
            imm_hmm_set_trans(mo.hmm, imm_super(d[k]), imm_super(e),
                              imm_log(0.2));
            imm_hmm_set_trans(mo.hmm, imm_super(i[k]), imm_super(e),
                              imm_log(0.2));
        }
    }

    return mo;
}

#if 0
void destroy_model(struct model model)
{
    for (size_t i = 0; i < imm_vecp_size(model.mute_states); ++i)
    {
        struct imm_mute_state const *s = imm_vecp_get(model.mute_states, i);
        imm_mute_state_destroy(s);
    }

    for (size_t i = 0; i < imm_vecp_size(model.normal_states); ++i)
    {
        struct imm_normal_state const *s = imm_vecp_get(model.normal_states, i);
        imm_normal_state_destroy(s);
    }

    imm_mute_state_destroy(model.end);
    imm_abc_destroy(model.abc);
    imm_hmm_destroy(model.hmm);
    imm_vecp_destroy(model.mute_states);
    imm_vecp_destroy(model.normal_states);
}
#endif

char const *get_model_str(void)
{
    static char const str[] =
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
        "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
        "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
        "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMME";

    return str;
}

#ifndef DP_UNSAFE_PAIR_H
#define DP_UNSAFE_PAIR_H

struct unsafe_pair
{
    unsigned src;
    unsigned dst;
    unsigned trans;
};

struct imm_dp;

unsigned find_unsafe_states(struct imm_dp const *, struct unsafe_pair *);

#endif

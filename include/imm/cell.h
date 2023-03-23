#ifndef IMM_CELL_H
#define IMM_CELL_H

struct imm_cell
{
    unsigned row;
    unsigned state;
    unsigned len;
};

static inline struct imm_cell imm_cell_init(unsigned row, unsigned state,
                                            unsigned len)
{
    return (struct imm_cell){.row = row, .state = state, .len = len};
}

#endif

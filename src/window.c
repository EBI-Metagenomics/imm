#include "imm/window.h"
#include "free.h"
#include "imm/report.h"
#include "imm/subseq.h"
#include "min.h"
#include <limits.h>
#include <stdbool.h>

struct imm_window
{
    struct imm_seq const* seq;
    unsigned              length;
    unsigned              offset;
    bool                  end;
};

struct imm_window* imm_window_create(struct imm_seq const* seq, unsigned length)
{
    length = MIN(length, imm_seq_length(seq));

    struct imm_window* window = malloc(sizeof(*window));
    window->seq = seq;
    window->length = length;
    window->offset = 0;
    window->end = false;

    return window;
}

void imm_window_destroy(struct imm_window const* window) { free_c(window); }

struct imm_subseq imm_window_get(struct imm_window const* window, unsigned index)
{
    unsigned const offset = MAX(window->length / 2, (unsigned)1) * index;
    unsigned const length = MIN(window->length, imm_seq_length(window->seq) - offset);

    IMM_SUBSEQ(subseq, window->seq, offset, length);

    return subseq;
}

unsigned imm_window_size(struct imm_window const* window)
{
    unsigned size = 0;
    unsigned offset = 0;

    unsigned length = 0;
    unsigned poffset = 0;
    do {
        poffset = offset;
        offset += MAX(window->length / 2, (unsigned)1);
        length = MIN(window->length, imm_seq_length(window->seq) - poffset);
        ++size;

    } while (imm_seq_length(window->seq) > poffset + length);

    return size;
}

#include "imm/window.h"
#include "free.h"
#include "imm/report.h"
#include "imm/subseq.h"
#include "min.h"
#include <limits.h>

struct imm_window
{
    struct imm_seq const* seq;
    unsigned              length;
    /* struct imm_subseq     subseq; */
    unsigned offset;
};

struct imm_window* imm_window_create(struct imm_seq const* seq, unsigned const length)
{
    if (length == 0) {
        imm_error("window length cannot be zero");
        return NULL;
    }
    if (imm_seq_length(seq) == 0) {
        imm_error("sequence length cannot be zero");
        return NULL;
    }

    struct imm_window* window = malloc(sizeof(struct imm_window));
    window->seq = seq;
    window->length = length;
    window->offset = 0;

    return window;
}

struct imm_subseq imm_window_next(struct imm_window* window)
{
    unsigned const offset = window->offset;
    unsigned const length = MIN(window->length, imm_seq_length(window->seq) - offset);

    IMM_SUBSEQ(subseq, window->seq, offset, length);

    if (offset >= imm_seq_length(window->seq)) {
        imm_error("there is no next windows");
        return subseq;
    }

    window->offset += MAX(window->length / 2, 1);
    if (imm_seq_length(window->seq) == offset + length)
        window->offset = UINT_MAX;

    return subseq;
}

bool imm_window_end(struct imm_window const* window)
{
    return window->offset >= imm_seq_length(window->seq);
}

void imm_window_destroy(struct imm_window const* window) { free_c(window); }

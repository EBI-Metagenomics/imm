#include "error.h"

#ifndef NDEBUG
#include <stdio.h>
#endif

#define NOT_AN_ERROR "not an error"

static char const strings[][44] = {
    [IMM_OK] = NOT_AN_ERROR,
    [IMM_NOMEM] = "not enough memory",
    [IMM_IOERROR] = "i/o error",
    [IMM_ANY_SYMBOL_OUT_RANGE] = "any_symbol outside range",
    [IMM_ANY_SYMBOL_IN_ABC] = "any_symbol cannot be in the alphabet",
    [IMM_EMPTY_ABC] = "alphabet cannot be empty",
    [IMM_TOO_MANY_SYMBOLS] = "symbols length is too large",
    [IMM_SYMBOL_OUT_OF_RANGE] = "symbol outside range",
    [IMM_DUPLICATED_SYMBOLS] = "alphabet cannot have duplicated symbols",
    [IMM_NUCLT_LPROB_IO_FAIL] = "failed to read/write nuclt_lprob",
    [IMM_TSORT_MUTE_CYLES] = "mute cycles are not allowed",
    [IMM_DIFFERENT_ABC] = "alphabets must be the same",
    [IMM_SEQ_OUT_OF_RANGE] = "sequence not in range",
    [IMM_TOO_MANY_TRANSITIONS] = "max number of trans has been reached",
    [IMM_STATE_ALREADY_IN_HMM] = "state already belongs to a hmm",
    [IMM_END_STATE_NOT_FOUND] = "end state not found",
    [IMM_START_STATE_NOT_FOUND] = "start state not found",
    [IMM_STATE_NOT_FOUND] = "state(s) not found",
    [IMM_TRANSITION_NOT_FOUND] = "transition not found",
    [IMM_PATH_NO_STEPS] = "path must have steps",
    [IMM_FIRST_STATE_NOT_STARTING] = "first state must be the starting one",
    [IMM_PATH_EMITS_MORE_SEQ] = "path emits more symbols than sequence",
    [IMM_SEQ_LONGER_THAN_PATH_SYMBOLS] = "sequence is longer than path symbols",
    [IMM_NON_FINITE_PROBABILITY] = "probability must be finite",
    [IMM_NAN_PROBABILITY] = "probability cannot be NaN",
    [IMM_NOT_SET_SEQ] = "sequence has not been set",
    [IMM_SEQ_TOO_SHORT] = "seq is shorter than end_state's lower bound",
    [IMM_ELAPSED_LIB_FAILED] = "elapsed library has failed",
};

enum imm_rc __imm_error_print(enum imm_rc rc, char const *ctx, char const *msg)
{
#ifdef NDEBUG
    (void)ctx;
    (void)msg;
#else
    fputs(ctx, stderr);
    fputs(": ", stderr);
    fputs(msg, stderr);
    fputc('\n', stderr);
    fflush(stderr);
#endif
    return rc;
}

char const *imm_error_string(enum imm_rc rc)
{
    return rc > IMM_LAST_RC ? NOT_AN_ERROR : strings[rc];
}

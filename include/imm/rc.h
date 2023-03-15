#ifndef IMM_RC_H
#define IMM_RC_H

enum imm_rc
{
    IMM_OK,
    IMM_NOMEM,
    IMM_IOERROR,
    IMM_ANY_SYMBOL_OUT_RANGE,
    IMM_ANY_SYMBOL_IN_ABC,
    IMM_EMPTY_ABC,
    IMM_TOO_MANY_SYMBOLS,
    IMM_SYMBOL_OUT_OF_RANGE,
    IMM_DUPLICATED_SYMBOLS,
    IMM_NUCLT_LPROB_IO_FAIL,
    IMM_TSORT_MUTE_CYLES,
    IMM_DIFFERENT_ABC,
    IMM_SEQ_OUT_OF_RANGE,
    IMM_TOO_MANY_TRANSITIONS,
    IMM_STATE_ALREADY_IN_HMM,
    IMM_END_STATE_NOT_FOUND,
    IMM_START_STATE_NOT_FOUND,
    IMM_STATE_NOT_FOUND,
    IMM_TRANSITION_NOT_FOUND,
    IMM_PATH_NO_STEPS,
    IMM_FIRST_STATE_NOT_STARTING,
    IMM_PATH_EMITS_MORE_SEQ,
    IMM_SEQ_LONGER_THAN_PATH_SYMBOLS,
    IMM_NON_FINITE_PROBABILITY,
    IMM_NAN_PROBABILITY,
    IMM_NOT_SET_SEQ,
    IMM_SEQ_TOO_SHORT,
    IMM_ELAPSED_LIB_FAILED,
    IMM_TOO_MANY_UNSAFE_STATES,
};

#define IMM_LAST_RC IMM_TOO_MANY_UNSAFE_STATES

#endif

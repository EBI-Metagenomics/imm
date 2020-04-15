#include "imm/state_factory.h"
#include "imm/state.h"
#include "mute_state.h"
#include "normal_state.h"
#include "state_factory.h"

int state_factory_read(FILE* stream, struct imm_state* state)
{
    switch (state->type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        if (mute_state_read(stream, state)) {
            imm_error("could not read mute_state");
            return 1;
        }
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (normal_state_read(stream, state)) {
            imm_error("could not read normal_state");
            return 1;
        }
        break;
    default:
        imm_error("unknown type_id");
        return 1;
    }
    return 0;
}

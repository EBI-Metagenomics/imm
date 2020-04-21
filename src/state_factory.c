#include "state_factory.h"
#include "imm/mute_state.h"
#include "imm/normal_state.h"
#include "imm/state.h"
#include "imm/state_types.h"
#include "mute_state.h"
#include "normal_state.h"

struct imm_state const* state_factory_read(FILE* stream, uint8_t type_id, struct imm_abc const* abc)
{
    struct imm_state const* state = NULL;

    switch (type_id) {
    case IMM_MUTE_STATE_TYPE_ID:
        if (!(state = mute_state_read(stream, abc)))
            imm_error("could not read mute_state");
        break;
    case IMM_NORMAL_STATE_TYPE_ID:
        if (!(state = normal_state_read(stream, abc)))
            imm_error("could not read normal_state");
        break;
    default:
        imm_error("unknown type_id");
    }
    return state;
}

#include "error.h"
#include "log.h"

enum imm_rc __error(enum imm_rc rc, char const *msg)
{
    log_print(msg);
    return rc;
}

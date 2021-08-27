#ifndef LOG_H
#define LOG_H

#include "imm/rc.h"

enum
{
    IMM_ERROR,
    IMM_FATAL
};

#define __imm_log(lvl, rc, msg)                                                \
    __imm_log_impl(lvl, rc, __FILE__ ":" IMM_XSTR(__LINE__) ":" #rc ": " msg)

enum imm_rc __imm_log_impl(int lvl, enum imm_rc rc, char const *msg);

#define __imm_error(rc, msg) __imm_log(IMM_ERROR, rc, msg)
#define __imm_fatal(rc, msg) __imm_log(IMM_FATAL, rc, msg)

#define error(rc, msg) __imm_error(rc, msg)
#define fatal(rc, msg) __imm_fatal(rc, msg)

#endif

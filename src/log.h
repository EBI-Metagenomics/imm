#ifndef LOG_H
#define LOG_H

#include "imm/rc.h"

enum
{
    LOG_ERROR,
    LOG_FATAL
};

#define __log(lvl, rc, msg)                                                    \
    __log_impl(lvl, rc, __FILE__ ":" IMM_XSTR(__LINE__) ":" #rc ": " msg)

enum imm_rc __log_impl(int lvl, enum imm_rc rc, char const *msg);

#define error(rc, msg) __log(LOG_ERROR, rc, msg)
#define fatal(rc, msg) __log(LOG_FATAL, rc, msg)

#endif

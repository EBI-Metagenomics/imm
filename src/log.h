#ifndef LOG_H
#define LOG_H

#include "imm/rc.h"

enum
{
    LOG_ERROR,
    LOG_FATAL
};

#define LOG_FMT(rc, msg) __FILE__ ":" IMM_XSTR(__LINE__) ":" #rc ": " msg

enum imm_rc __log_it(int lvl, enum imm_rc rc, char const *msg);

#define error(rc, msg) __log_it(LOG_ERROR, rc, LOG_FMT(rc, msg))
#define fatal(rc, msg) __log_it(LOG_FATAL, rc, LOG_FMT(rc, msg))

#endif

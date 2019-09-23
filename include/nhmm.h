#ifndef NHMM_H_API
#define NHMM_H_API

#define NHMM_VERSION_MAJOR 0
#define NHMM_VERSION_MINOR 0
#define NHMM_VERSION_PATCH 1

/* Final version. */
#define _NHMM_VERSION NHMM_VERSION_MAJOR.NHMM_VERSION_MINOR.NHMM_VERSION_PATCH
#define _NHMM_QUOTE(str) #str
#define _NHMM_EXPAND_AND_QUOTE(str) _NHMM_QUOTE(str)
#define NHMM_VERSION _NHMM_EXPAND_AND_QUOTE(_NHMM_VERSION)

#ifdef __cplusplus
extern "C"
{
#endif

#include "nhmm/alphabet.h"
#include "nhmm/api.h"
#include "nhmm/hmm.h"
#include "nhmm/state.h"

#ifdef __cplusplus
}
#endif

#endif

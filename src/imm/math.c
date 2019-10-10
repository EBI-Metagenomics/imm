#include "imm.h"
#include <math.h>

int imm_isninf(double a) { return isinf(a) && a < 0.0; }

#ifndef ALIB_C_RBUFFIT_PRIVATE_IS_DEFINED
#define ALIB_C_RBUFFIT_PRIVATE_IS_DEFINED

#include "RBuffIt.h"
#include "RBuff_private.h"

/* Iterator object for RBuffs. */
struct RBuffIt
{
	RBuff* rbuff;
	void* it;
};

#endif

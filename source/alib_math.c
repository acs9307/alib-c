#include "includes/alib_math.h"

/* Returns 1 if the value is prime, 0 otherwise. */
char isPrime(uint64_t x)
{
	uint32_t halfX = (x / 2) + 1;

	for (uint64_t i = 2; i < halfX; ++i)
	{
		if (x % i == 0)
			return(0);
	}

	return(1);
}
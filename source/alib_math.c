#include "includes/alib_math.h"

/* Returns 1 if the value is prime, 0 otherwise. */
char isPrime(int64_t x)
{
	int64_t halfX = (x / 2);
	if (halfX < 0)
	{
		halfX *= -1;
	}
	halfX += 1;

	for (int64_t i = 2; i < halfX; ++i)
	{
		if (x % i == 0)
			return(0);
	}

	return(1);
}
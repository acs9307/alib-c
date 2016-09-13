#include "Endianess.h"

ENDIAN_TYPE ENDIANESS = ENDIAN_UNKNOWN;

ENDIAN_TYPE check_endianess()
{
#ifdef DYNAMIC_ENDIANESS
	if (!ENDIANESS)
	{
		uint16_t x = 1;
		uint8_t* y = (uint8_t*)&x;
		if (*y == 1)
			ENDIANESS = ENDIAN_LITTLE;
		else
			ENDIANESS = ENDIAN_BIG;
	}
#endif
	return(ENDIANESS);
}

void flip_endianess(void* itm, uint8_t size)
{
	uint8_t* it_begin = (uint8_t*)itm;
	uint8_t* it_end = (uint8_t*)itm + size - 1;  //Subtract one so we will point to the last byte.
	uint8_t holder;

	for (; it_begin < it_end; ++it_begin, ++it_end)
	{
		holder = *it_begin;
		*it_begin = *it_end;
		*it_end = holder;
	}
}
#ifndef ENDIANESS_IS_DEFINED
#define ENDIANESS_IS_DEFINED

#include <inttypes.h>

#define DYNAMIC_ENDIANESS

typedef enum ENDIAN_TYPE
{
	ENDIAN_UNKNOWN = 0,
	ENDIAN_BIG = 1,
	ENDIAN_LITTLE = 2
}ENDIAN_TYPE;

extern ENDIAN_TYPE ENDIANESS;

ENDIAN_TYPE check_endianess();

void flip_endianess(void* itm, uint8_t size);

#ifdef DYNAMIC_ENDIANESS

/* Transforms a block of data into big endian.
*
* Parameters:
*		itm (void*): Pointer to the block of data to modify.
*		size (uint8_t): The size of the block of memory in bytes. */
#define TO_LITTLE_ENDIAN(itm, size) do{ \
	check_endianess(); \
	if(ENDIANESS == ENDIAN_BIG)flip_endianess(itm, size); \
	}while(0)

/* Transforms a block of data into little endian.
*
* Parameters:
*		itm (void*): Pointer to the block of data to modify.
*		size (uint8_t): The size of the block of memory in bytes. */
#define TO_BIG_ENDIAN(itm, size) do{ \
	check_endianess(); \
	if(ENDIANESS == ENDIAN_LITTLE)flip_endianess(itm, size); \
	}while(0)

#else

/* Transforms a block of data into big endian.
*
* Parameters:
*		itm (void*): Pointer to the block of data to modify.
*		size (uint8_t): The size of the block of memory in bytes. */
#define TO_LITTLE_ENDIAN(itm, size) \
	if(ENDIANESS == ENDIAN_BIG)flip_endianess(itm, size)

/* Transforms a block of data into little endian.
*
* Parameters:
*		itm (void*): Pointer to the block of data to modify.
*		size (uint8_t): The size of the block of memory in bytes. */
#define TO_BIG_ENDIAN(itm, size) \
	if(ENDIANESS == ENDIAN_LITTLE)flip_endianess(itm, size)

#endif

#endif

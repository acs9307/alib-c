#ifndef ZIP_ITER_PRIVATE_IS_DEFINED
#define ZIP_ITER_PRIVATE_IS_DEFINED

#include "ZipIter.h"

#define ZIP_ITER_MEMBERS 		\
	z_stream strm;				\
	uint8_t in_buff[ZIP_CHUNK]

struct ZipIter
{
	ZIP_ITER_MEMBERS;
};

#endif

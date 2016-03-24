#ifndef ZIP_FILE_ITER_PRIVATE_IS_DEFINED
#define ZIP_FILE_ITER_PRIVATE_IS_DEFINED

#include "ZipFileIter.h"
#include "ZipIter_private.h"

struct ZipFileIter
{
	ZIP_ITER_MEMBERS;
	FILE* file;
	size_t to_pos;
};

#endif

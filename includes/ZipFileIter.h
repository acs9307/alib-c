#ifndef ZIP_FILE_ITER_IS_DEFINED
#define ZIP_FILE_ITER_IS_DEFINED

#include "ZipIter.h"

typedef struct ZipFileIter ZipFileIter;

/* Public Functions */
	/* Constructors */
ZipFileIter* newZipFileIter_deflate(int level, FILE* file, size_t to_pos);
	/****************/

	/* Destructors */
void freeZipFileIter_deflate_ex(ZipFileIter* zfi, char close_file);
void freeZipFileIter_deflate(ZipFileIter* zfi);
void delZipFileIter_deflate_ex(ZipFileIter** zfi, char close_file);
void delZipFileIter_deflate(ZipFileIter** zfi);

		/* Callback Functions */
void freeZipFileIter_deflate_cb_close_file(void* zfi_void);
void freeZipFileIter_deflate_cb(void* zfi_void);
		/*********************/
	/***************/
/********************/

#endif

#include "includes/ZipFileIter_private.h"

/* Public Functions */
	/* Constructors */
ZipFileIter* newZipFileIter_deflate(int level, FILE* file, size_t to_pos)
{
	if(!file)return(NULL);

	ZipFileIter* zfi = malloc(sizeof(ZipFileIter));
	if(!zfi)return(NULL);

	zfi->file = file;
	if(to_pos == 0)
		zfi->to_pos = SIZE_MAX;
	else
		zfi->to_pos = to_pos;

	if(init_deflate_stream(&zfi->strm, level) != Z_OK)
		delZipFileIter_deflate_ex(&zfi, 0);

	return(zfi);
}
	/****************/

	/* Destructors */
void freeZipFileIter_deflate_ex(ZipFileIter* zfi, char close_file)
{
	if(!zfi)return;

	if(close_file && zfi->file)
		fclose(zfi->file);
	deflateEnd(&zfi->strm);

	free(zfi);
}
void freeZipFileIter_deflate(ZipFileIter* zfi)
{
	freeZipFileIter_deflate_ex(zfi, 1);
}
void delZipFileIter_deflate_ex(ZipFileIter** zfi, char close_file)
{
	if(!zfi || !*zfi)return;

	freeZipFileIter_deflate_ex(*zfi, close_file);
	*zfi = NULL;
}
void delZipFileIter_deflate(ZipFileIter** zfi)
{
	delZipFileIter_deflate_ex(zfi, 1);
}

		/* Callback Functions */
void freeZipFileIter_deflate_cb_close_file(void* zfi_void)
{
	freeZipFileIter_deflate_ex((ZipFileIter*)zfi_void, 1);
}
void freeZipFileIter_deflate_cb(void* zfi_void)
{
	freeZipFileIter_deflate_ex((ZipFileIter*)zfi_void, 0);
}
		/*********************/
	/***************/
/********************/

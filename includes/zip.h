#ifndef ZIP_IS_DEFINED
#define ZIP_IS_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#include "alib_error.h"
#include "BinaryBuffer.h"

#define ZIP_CHUNK 64*1024

/*******Stream Initializers*******/
/* Initializes a 'z_stream' for deflation. */
int init_deflate_stream(z_stream* strm, int level);
/* Initializer for a gzip deflation. */
int init_gzip_deflate(z_stream* strm, int level);

/* Initializes a 'z_stream' for inflation. */
int init_inflate_stream(z_stream* strm);
/*********************************/

/* Deflates an array of data, and places deflated data into a buffer.
 * This is the fastest deflate function as all data is stored in RAM.
 * This is suitable for small files.  For large files, it is suggested to use
 * 'deflate_btf()' or 'deflate_ftf()' to prevent such a large memory footprint.
 *
 * Parameters:
 * 		buff: The buffer storing the data to deflate.
 * 		buff_len: The length of 'buff' in bytes.
 * 		out_buff: A double pointer that will be set to a newly allocated array of
 * 			deflated data.  THIS MUST be freed by the caller.
 * 		out_buff_len: A pointer to an unsigned integer that will store the length
 * 			of 'out_buff' in bytes.
 * 		level: The level to deflate. Usually 'Z_DEFAULT_COMPRESSION'.
 *
 * Returns a Z_LIB error code. */
int deflate_btb(const unsigned char* buff, size_t buff_len, unsigned char** out_buff,
		size_t* out_buff_len, int level);
/* Deflates a file and places deflated data into an array.
 * This is quite fast, but not as fast as 'deflate_btb()', however it comes
 * with the advantage of having a smaller memory footprint than 'deflate_btb()'.
 * This is suitable for small to medium sized files.  For large files, it is
 * suggested to use 'deflate_ftf()' decrease the memory footprint.
 *
 * Parameters:
 * 		source: The file to deflate.
 * 		out_buff: A double pointer that will be set to a newly allocated array of
 * 			deflated data.  THIS MUST be freed by the caller.
 * 		out_buff_len: A pointer to an unsigned integer that will store the length
 * 			of 'out_buff' in bytes.
 * 		level: The level to deflate. Usually 'Z_DEFAULT_COMPRESSION'.
 *
 * Returns a Z_LIB error code. */
int deflate_ftb(FILE* source, unsigned char** out_buff, size_t* out_buff_len,
		int level);
/* Deflates a buffer and places deflated data into a file.
 * This is quite fast, but not as fast as 'deflate_btb()', however it comes
 * with the advantage of having a smaller memory footprint than 'deflate_btb()'.
 * This is suitable for small to medium sized files.  For large files, it is
 * suggested to use 'deflate_ftf()' decrease the memory footprint.
 *
 * This should only be used if the source file has already been loaded into memory,
 * otherwise it is suggested to use 'deflate_ftf()' or 'deflate_ftb()' instead.
 *
 * Parameters:
 * 		buff: Buffer containing data that is to be deflated.
 * 		out_buff_len: The length of 'buff' in bytes.
 * 		dest: The file to write deflated data to.
 * 		level: The level to deflate. Usually 'Z_DEFAULT_COMPRESSION'.
 *
 * Returns a Z_LIB error code. */
int deflate_btf(const unsigned char* buff, size_t buff_len, FILE* dest, int level);
/* Deflates a file and places deflated data into the output file.
 * Slowest deflate function, however requires the least memory.
 * This is suitable for any sized files.
 *
 * Parameters:
 * 		source: The file to deflate.
 * 		dest: The file to write deflated data to.
 * 		level: The level to deflate. Usually 'Z_DEFAULT_COMPRESSION'.
 *
 * Returns a Z_LIB error code. */
int deflate_ftf(FILE* source, FILE* dest, int level);

/* Inflates the data in a buffer and stores inflated data in 'out_buff'.
 *
 * Suitable for small files.  For larger files, it is suggested to use
 * 'inflate_btf()' or 'inflate_ftf()'.
 *
 * Parameters:
 * 		buff: The buffer of deflated data.
 * 		buff_len: The length of 'buff' in bytes.
 * 		out_buff: The buffer that will store inflated data.  This is dynamically allocated
 * 			within the function and MUST BE FREED by the caller.  If the pointer
 * 			is already pointing to allocated memory before the call to this function,
 * 			the old memory WILL NOT be freed.
 * 		out_buff_len: Pointer to the length of 'out_buff' in bytes.
 *
 * Returns Z_LIB error code. */
int inflate_btb(const unsigned char* buff, size_t buff_len, unsigned char** out_buff,
		size_t* out_buff_len);
/* Inflates the data in a file and stores inflated data in 'out_buff'.
 *
 * Suitable for small to medium files.  For larger files, it is suggested to use
 * 'inflate_ftf()' to help decrease memory footprint.
 *
 * Parameters:
 * 		source: The deflated file.
 * 		out_buff: The buffer that will store inflated data.  This is dynamically allocated
 * 			within the function and MUST BE FREED by the caller.  If the pointer
 * 			is already pointing to allocated memory before the call to this function,
 * 			the old memory WILL NOT be freed.
 * 		out_buff_len: Pointer to the length of 'out_buff' in bytes.
 *
 * Returns Z_LIB error code. Returns Z_OK on success. */
int inflate_ftb(FILE* source, unsigned char** out_buff, size_t* out_buff_len);
/* Inflates the data in a buffer and stores inflated data in a file.
 *
 * Suitable for small to medium files.  For larger files, it is suggested to use
 * 'inflate_ftf()' to help decrease memory footprint.
 *
 * Parameters:
 * 		buff: The buffer of deflated data.
 * 		buff_len: The length of 'buff' in bytes.
 * 		dest: The file to write inflated data to.
 *
 * Returns Z_LIB error code. Returns Z_OK on success. */
int inflate_btf(const unsigned char* buff, size_t buff_len, FILE* dest);
/* Inflates the data in a file and stores inflated data in a file.
 *
 * Suitable for small to medium files.  For larger files, it is suggested to use
 * 'inflate_ftf()' to help decrease memory footprint.
 *
 * Parameters:
 * 		src: The file to inflate.
 * 		dest: The file to write inflated data to.
 *
 * Returns Z_LIB error code. Returns Z_OK on success. */
int inflate_ftf(FILE* src, FILE* dest);

#endif

#include "includes/zip.h"
#include "includes/BinaryBuffer_private.h"

/* Initializes a 'z_stream' for deflation. */
int init_deflate_stream(z_stream* strm, int level)
{
	if(!strm)return(ALIB_BAD_ARG);

	strm->zalloc = Z_NULL;
	strm->zfree = Z_NULL;
	strm->opaque = Z_NULL;
	return(deflateInit(strm, level));
}
/* Initializer for a gzip deflation. */
int init_gzip_deflate(z_stream* strm, int level)
{
	if(!strm)return(ALIB_BAD_ARG);

	strm->zalloc = Z_NULL;
	strm->zfree = Z_NULL;
	strm->opaque = Z_NULL;
	strm->avail_out = strm->avail_in = 0;
	strm->next_out = strm->next_in = NULL;

	return(deflateInit2(strm, level, Z_DEFLATED,
			MAX_WBITS+16, 8, Z_DEFAULT_STRATEGY));
}

/* Initializes a 'z_stream' for inflation. */
int init_inflate_stream(z_stream* strm)
{
	if(!strm)return(ALIB_BAD_ARG);

	strm->zalloc = Z_NULL;
	strm->zfree = Z_NULL;
	strm->opaque = Z_NULL;
	strm->avail_in = 0;
	strm->next_in = Z_NULL;
	return(inflateInit(strm));
}

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
		size_t* out_buff_len, int level)
{
	int rval = 0;
	z_stream strm;
	/* Temporarily stores deflated data. */
	BinaryBuffer* out;

	/* Check for errors. */
	if(!buff || !out_buff || !buff_len || !out_buff_len)
		return(Z_BUF_ERROR);

	/* Create the buffer to store converted data in. */
	out = newBinaryBuffer_ex(NULL, 0, 0, 0, 0);
	if(!out)return(Z_MEM_ERROR);

	/* Initialize zip stream. */
	rval = init_deflate_stream(&strm, level);
	if(rval)return(rval);

	strm.next_in = (unsigned char*)buff;
	strm.avail_in = buff_len;

	/* Deflate and add deflated data to the buffer until there is
	 * nothing left in the original buffer. */
	do
	{
		BinaryBuffer_hard_resize(out, out->len + ZIP_CHUNK);
		strm.avail_out = ZIP_CHUNK;
		strm.next_out = (unsigned char*)BinaryBuffer_get_raw_buff(out) +
				out->len;
		rval = deflate(&strm, Z_FINISH);
		if(rval == Z_STREAM_ERROR)
			goto f_return;

		out->len += ZIP_CHUNK - strm.avail_out;
	}while(strm.avail_out == 0);

	/* If we reached the end of the stream, then all is good,
	 * change the return value accordingly. */
	if(rval == Z_STREAM_END)
		rval = Z_OK;

	/* Set the output parameters as needed. */
	BinaryBuffer_shrink_to_fit(out);
	*out_buff_len = out->len;
	*out_buff = BinaryBuffer_extract_buffer(out);

f_return:
	delBinaryBuffer(&out);
	deflateEnd(&strm);

	return(rval);
}
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
int deflate_ftb(FILE* source, unsigned char** out_buff, size_t* out_buff_len, int level)
{
	int rval = 0, flush;
	z_stream strm;
	unsigned char in[ZIP_CHUNK];
	BinaryBuffer* out;

	/* Check for errors. */
	if(!source || !out_buff || !out_buff_len)
		return(Z_BUF_ERROR);

	/* Initialize deflated buffer. */
	out = newBinaryBuffer_ex(NULL, 0, 0, 0, 0);
	if(!out)return(Z_MEM_ERROR);

	/* Initialize zip stream. */
	rval = init_deflate_stream(&strm, level);
	if(rval)return(rval);

	/* Continue until there is nothing left in the file. */
	do
	{
		/* Read data from the file. */
		strm.avail_in = fread(in, 1, ZIP_CHUNK, source);
		if(ferror(source))
		{
			rval = Z_ERRNO;
			goto f_return;
		}
		flush = feof(source)?Z_FINISH:Z_NO_FLUSH;
		strm.next_in = in;

		/* Continue until all the data in the file buffer
		 * has been compressed and stored in the output buffer. */
		do
		{
			BinaryBuffer_hard_resize(out, out->len + ZIP_CHUNK);
			strm.avail_out = ZIP_CHUNK;
			strm.next_out = (unsigned char*)BinaryBuffer_get_raw_buff(out) +
					out->len;
			rval = deflate(&strm, flush);
			if(rval == Z_STREAM_ERROR)
				goto f_return;

			out->len += ZIP_CHUNK - strm.avail_out;
		}while(strm.avail_out == 0);
	}while(flush != Z_FINISH);

	/* If the end of the stream is hit, then all is good.
	 * Modify 'rval' to reflect the status. */
	if(rval == Z_STREAM_END)
		rval = Z_OK;

	/* Set output parameters to the compressed buffer's data. */
	BinaryBuffer_shrink_to_fit(out);
	*out_buff_len = out->len;
	*out_buff = BinaryBuffer_extract_buffer(out);

f_return:
	delBinaryBuffer(&out);
	deflateEnd(&strm);

	return(rval);
}
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
int deflate_btf(const unsigned char* buff, size_t buff_len, FILE* dest, int level)
{
	int rval = 0, have;
	z_stream strm;
	unsigned char out[ZIP_CHUNK];

	/* Check for errors. */
	if(!buff || !buff_len || !dest)
		return(Z_BUF_ERROR);

	/* Initialize zip stream. */
	rval = init_deflate_stream(&strm, level);
	if(rval)return(rval);

	/* Set the input data for the stream. */
	strm.next_in = (unsigned char*)buff;
	strm.avail_in = buff_len;

	/* Continue deflating until all the data has been
	 * deflated and written to the output file. */
	do
	{
		strm.avail_out = ZIP_CHUNK;
		strm.next_out = out;
		rval = deflate(&strm, Z_FINISH);
		if(rval == Z_STREAM_ERROR)
			goto f_return;

		have = ZIP_CHUNK - strm.avail_out;
		if(fwrite(out, 1, have, dest) != have || ferror(dest))
		{
			rval = Z_ERRNO;
			goto f_return;
		}
	}while(strm.avail_out == 0);

	/* If we hit the end of the input stream, then all is good.
	 * Modify 'rval' to show this. */
	if(rval == Z_STREAM_END)
		rval = Z_OK;

f_return:
	deflateEnd(&strm);

	return(rval);
}
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
int deflate_ftf(FILE* source, FILE* dest, int level)
{
	int rval = 0, flush;
	size_t have;
	z_stream strm;
	unsigned char in[ZIP_CHUNK];
	unsigned char out[ZIP_CHUNK];

	/* Check for errors. */
	if(!source || !dest)
		return(Z_BUF_ERROR);

	/* Initialize zip stream. */
	rval = init_deflate_stream(&strm, level);
	if(rval)return(rval);

	/* Read and compress a chunk per iteration. */
	do
	{
		strm.avail_in = fread(in, 1, ZIP_CHUNK, source);
		if(ferror(source))
		{
			rval = Z_ERRNO;
			goto f_return;
		}
		flush = feof(source)?Z_FINISH:Z_NO_FLUSH;
		strm.next_in = in;

		/* Compress then store compressed data in
		 * output file. */
		do
		{
			strm.avail_out = ZIP_CHUNK;
			strm.next_out = out;
			rval = deflate(&strm, flush);
			if(rval == Z_STREAM_ERROR)
				goto f_return;

			have = ZIP_CHUNK - strm.avail_out;
			if(fwrite(out, 1, have, dest) != have || ferror(dest))
			{
				rval = Z_ERRNO;
				goto f_return;
			}
		}while(strm.avail_out == 0);
	}while(flush != Z_FINISH);

	/* If the end of the input stream is hit, everything was successful. */
	if(rval == Z_STREAM_END)
		rval = Z_OK;

f_return:
	deflateEnd(&strm);
	return(rval);
}

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
int inflate_btb(const unsigned char* buff, size_t buff_len, unsigned char** out_buff, size_t* out_buff_len)
{
	int rval;
	z_stream strm;
	BinaryBuffer* out;

	/* Check for errors. */
	if(!buff || !buff_len || !out_buff || !out_buff_len)
		return(Z_DATA_ERROR);

	/* Initialize the output buffer. */
	out = newBinaryBuffer_ex(NULL, 0, 0, 0, 0);
	if(!out)return(Z_MEM_ERROR);

	/* Initialize zip stream. */
	rval = init_inflate_stream(&strm);
	if(rval)return(rval);

	/* Set the stream's input buffer. */
	strm.avail_in = buff_len;
	strm.next_in = (unsigned char*)buff;

	/* Continue until all the data is deflated and the
	 * deflated data is placed in the output buffer. */
	do
	{
		BinaryBuffer_hard_resize(out, out->len + ZIP_CHUNK);
		strm.avail_out = ZIP_CHUNK;
		strm.next_out = (unsigned char*)BinaryBuffer_get_raw_buff(out) +
				out->len;

		rval = inflate(&strm, Z_NO_FLUSH);
		if(rval == Z_STREAM_ERROR)
			goto f_return;

		switch(rval)
		{
		case Z_NEED_DICT:
			rval = Z_DATA_ERROR;
			goto f_return;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			goto f_return;
		}

		out->len += ZIP_CHUNK - strm.avail_out;
	}while(strm.avail_out == 0);

	/* Hitting the end of stream is desired behavior. */
	if(rval == Z_STREAM_END)
		rval = Z_OK;

	/* Set output pointers. */
	BinaryBuffer_shrink_to_fit(out);
	*out_buff_len = out->len;
	*out_buff = BinaryBuffer_extract_buffer(out);

f_return:
	inflateEnd(&strm);
	delBinaryBuffer(&out);

	return(rval);
}
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
int inflate_ftb(FILE* source, unsigned char** out_buff, size_t* out_buff_len)
{
	int rval;
	z_stream strm;
	BinaryBuffer* out;
	unsigned char in[ZIP_CHUNK];

	/* Check for errors. */
	if(!source || !out_buff || !out_buff_len)
		return(Z_DATA_ERROR);

	/* Initialize the buffer to store the inflated data in. */
	out = newBinaryBuffer_ex(NULL, 0, 0, 0, 0);
	if(!out)return(Z_MEM_ERROR);

	/* Initialize zip stream. */
	rval = init_inflate_stream(&strm);
	if(rval)return(rval);

	/* Continue until all the data in 'source' has been read. */
	do
	{
		strm.avail_in = fread(in, 1, ZIP_CHUNK, source);
		if(ferror(source))
		{
			rval = Z_ERRNO;
			goto f_return;
		}
		if(strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* Continue until the chunk of data read from 'source'
		 * has been inflated and the inflated data has been stored
		 * into the output buffer. */
		do
		{
			BinaryBuffer_hard_resize(out, out->len + ZIP_CHUNK);
			strm.avail_out = ZIP_CHUNK;
			strm.next_out = (unsigned char*)BinaryBuffer_get_raw_buff(out) +
					out->len;

			rval = inflate(&strm, Z_NO_FLUSH);
			if(rval == Z_STREAM_ERROR)
				goto f_return;

			switch(rval)
			{
			case Z_NEED_DICT:
				rval = Z_DATA_ERROR;
				goto f_return;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				goto f_return;
			}

			out->len += ZIP_CHUNK - strm.avail_out;
		}while(strm.avail_out == 0);
	}while(rval != Z_STREAM_END);

	/* Hitting the end of the stream is desired behavior. */
	if(rval == Z_STREAM_END)
		rval = Z_OK;

	/* Set output parameter pointers. */
	BinaryBuffer_shrink_to_fit(out);
	*out_buff_len = out->len;
	*out_buff = BinaryBuffer_extract_buffer(out);

f_return:
	inflateEnd(&strm);
	delBinaryBuffer(&out);

	return(rval);
}
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
int inflate_btf(const unsigned char* buff, size_t buff_len, FILE* dest)
{
	int rval;
	size_t have;
	z_stream strm;
	unsigned char out[ZIP_CHUNK];

	/* Check for errors. */
	if(!buff || !buff_len || !dest)
		return(Z_DATA_ERROR);

	/* Initialize zip stream. */
	rval = init_inflate_stream(&strm);
	if(rval)return(rval);

	strm.next_in = (unsigned char*)buff;
	strm.avail_in = buff_len;

	/* Continue until all data has been inflated and inflated
	 * data has been stored into the output buffer. */
	do
	{
		strm.avail_out = ZIP_CHUNK;
		strm.next_out = out;

		rval = inflate(&strm, Z_NO_FLUSH);
		if(rval == Z_STREAM_ERROR)
			goto f_return;

		switch(rval)
		{
		case Z_NEED_DICT:
			rval = Z_DATA_ERROR;
			goto f_return;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			goto f_return;
		}

		have = ZIP_CHUNK - strm.avail_out;
		if(fwrite(out, 1, have, dest) != have || ferror(dest))
		{
			rval = Z_ERRNO;
			goto f_return;
		}
	}while(strm.avail_out == 0);

	/* Hitting the end of the input stream is desired behavior. */
	if(rval == Z_STREAM_END)
		rval = Z_OK;

f_return:
	inflateEnd(&strm);
	return(rval);
}
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
int inflate_ftf(FILE* src, FILE* dest)
{
	int rval;
	size_t have;
	z_stream strm;
	unsigned char in[ZIP_CHUNK];
	unsigned char out[ZIP_CHUNK];

	/* Check for errors. */
	if(!src || !dest)
		return(Z_DATA_ERROR);

	/* Initialize zip stream. */
	rval = init_inflate_stream(&strm);
	if(rval)return(rval);

	/* Read data in chunks from the source file until
	 * there is no more data available. */
	do
	{
		strm.avail_in = fread(in, 1, ZIP_CHUNK, src);
		if(ferror(src))
		{
			rval = Z_ERRNO;
			goto f_return;
		}
		if(strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* Inflate the data read from the source file and
		 * place inflated data into the output buffer. */
		do
		{
			strm.avail_out = ZIP_CHUNK;
			strm.next_out = out;

			rval = inflate(&strm, Z_NO_FLUSH);
			if(rval == Z_STREAM_ERROR)
				goto f_return;

			switch(rval)
			{
			case Z_NEED_DICT:
				rval = Z_DATA_ERROR;
				goto f_return;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				goto f_return;
			}

			have = ZIP_CHUNK - strm.avail_out;
			if(fwrite(out, 1, have, dest) != have || ferror(dest))
			{
				rval = Z_ERRNO;
				goto f_return;
			}
		}while(strm.avail_out == 0);
	}while(rval != Z_STREAM_END);

	/* Hitting the end of the stream is desired behavior. */
	if(rval == Z_STREAM_END)
		rval = Z_OK;

f_return:
	inflateEnd(&strm);
	return(rval);
}

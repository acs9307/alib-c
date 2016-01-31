#include <stdio.h>
#include <string.h>
#include "zip.h"
#include "BinaryBuffer.h"

int main(int argc, char **argv)
{
    int ret = 0;
    FILE* in, *out;

    /* do compression if no arguments */
#if 0
    {
    	in = fopen("/home/gurtha/zip_test/socket-test.c", "r");
    	out = fopen("/home/gurtha/zip_test/socket_test.zip", "w");

#if 1
    	printf("deflate_ftf: %d\n", deflate_ftf(in, out, Z_DEFAULT_COMPRESSION));
#endif

    	/* Test deflate_btb. */
#if 0
    	{
    		BinaryBuffer* bb_in = newBinaryBuffer();
    		unsigned char in_buff[ZIP_CHUNK];
    		unsigned char* out_buff;
    		size_t out_len = 0;

    		while(!feof(in))
    		{
    			int read_count = fread(in_buff, 1, ZIP_CHUNK, in);
    			if(read_count < 0)
    			{
    				printf("Problem reading from input file.\n");
    				goto f_return;
    			}

    			BinaryBuffer_append(bb_in, in_buff, read_count);
    		}
    		printf("deflate_btb: %d\n", deflate_btb(BinaryBuffer_get_raw_buff(bb_in),
    				BinaryBuffer_get_length(bb_in), &out_buff, &out_len,
					Z_BEST_COMPRESSION));
    		printf("out_len: %d\n", (int)out_len);

    		delBinaryBuffer(&bb_in);
    		free(out_buff);
    	}
#endif

#if 0
    	/* Test deflate_ftb(). */
    	{
    		unsigned char* out_buff;
    		size_t out_len;

    		printf("deflate_ftb: %d\n",
    				deflate_ftb(in, &out_buff, &out_len, Z_BEST_COMPRESSION));
    		printf("out_len: %d\n", (int)out_len);

    		free(out_buff);
    	}
#endif

#if 0
    	/* Test deflate_btf(). */
    	{
    		BinaryBuffer* bb_in = newBinaryBuffer();
    		unsigned char in_buff[ZIP_CHUNK];

    		while(!feof(in))
    		{
    			int read_count = fread(in_buff, 1, ZIP_CHUNK, in);
    			if(read_count < 0)
    			{
    				printf("Problem reading from input file.\n");
    				goto f_return;
    			}

    			BinaryBuffer_append(bb_in, in_buff, read_count);
    		}
    		printf("deflate_btf: %d\n", deflate_btf(BinaryBuffer_get_raw_buff(bb_in),
    				BinaryBuffer_get_length(bb_in), out,
					Z_BEST_COMPRESSION));

    		delBinaryBuffer(&bb_in);
    	}
#endif
    }
#else
    /* do decompression if -d specified */
    {
    	in = fopen("/home/gurtha/zip_test/socket_test.zip", "r");
    	out = fopen("/home/gurtha/zip_test/socket_test_inflated", "w");

#if 0
    	printf("inflate_ftf: %d\n", inflate_ftf(in, out));
#endif

#if 0
    	{
    		BinaryBuffer* bb_in = newBinaryBuffer();
    		unsigned char in_buff[ZIP_CHUNK];
    		unsigned char* out_buff;
    		size_t out_len = 0;

    		while(!feof(in))
    		{
    			int read_count = fread(in_buff, 1, ZIP_CHUNK, in);
    			if(read_count < 0)
    			{
    				printf("Problem reading from input file.\n");
    				goto f_return;
    			}

    			BinaryBuffer_append(bb_in, in_buff, read_count);
    		}
    		printf("inflate_btb: %d\n", inflate_btb(BinaryBuffer_get_raw_buff(bb_in),
    				BinaryBuffer_get_length(bb_in), &out_buff, &out_len));
    		printf("out_len: %d\n", (int)out_len);

    		delBinaryBuffer(&bb_in);
    		free(out_buff);
    	}
#endif

#if 0
    	/* Test inflate_ftb(). */
    	{
    		unsigned char* out_buff;
    		size_t out_len;

    		printf("inflate_ftb: %d\n",
    				inflate_ftb(in, &out_buff, &out_len));
    		printf("out_len: %d\n", (int)out_len);

    		free(out_buff);
    	}
#endif

#if 1
    	/* Test inflate_btf(). */
    	{
    		BinaryBuffer* bb_in = newBinaryBuffer();
    		unsigned char in_buff[ZIP_CHUNK];

    		while(!feof(in))
    		{
    			int read_count = fread(in_buff, 1, ZIP_CHUNK, in);
    			if(read_count < 0)
    			{
    				printf("Problem reading from input file.\n");
    				goto f_return;
    			}

    			BinaryBuffer_append(bb_in, in_buff, read_count);
    		}
    		printf("inflate_btf: %d\n", inflate_btf(BinaryBuffer_get_raw_buff(bb_in),
    				BinaryBuffer_get_length(bb_in), out));

    		delBinaryBuffer(&bb_in);
    	}
#endif
    }
#endif

f_return:
	if(in)fclose(in);
	if(out)fclose(out);

	printf("application closing!\n");
	return(ret);
}


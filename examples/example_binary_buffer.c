#include <stdio.h>

#include "BinaryBuffer.h"

void print_buffer(BinaryBuffer* buff)
{
	printf("buff->buff: ");
	fwrite((char*)BinaryBuffer_get_raw_buff(buff), 1, BinaryBuffer_get_length(buff), stdout);
	printf("\n");

	printf("BinaryBuffer_length(): %d\n", (int)BinaryBuffer_get_length(buff));
	printf("BinaryBuffer_capacity(): %d\n", (int)BinaryBuffer_get_capacity(buff));
	printf("\n");
}

int main()
{
	BinaryBuffer* buff = newBinaryBuffer();
	char temp_buff[12] = {0};

	if(!buff)
	{
		printf("Could not created new buffer.\n");
		return(1);
	}

	/*-----------------------------------------*/
	printf("\tAPPEND\n");
	printf("BinaryBuffer_append(): %d\n",
			BinaryBuffer_append(buff, "hello world", 11));
	print_buffer(buff);

	printf("BinaryBuffer_append(): %d\n",
			BinaryBuffer_append(buff, "wassup.....", 11));
	print_buffer(buff);
	printf("\n\n");
	/*-----------------------------------------*/

	/*-----------------------------------------*/
	printf("\tREMOVE\n");
	printf("BinaryBuffer_remove(): %d\n", BinaryBuffer_remove(buff, 5, 15));
	print_buffer(buff);
	printf("\n\n");
	/*-----------------------------------------*/

	/*-----------------------------------------*/
	printf("\tCOPY\n");
	printf("BinaryBuffer_copy(): %d\n", BinaryBuffer_copy(buff, temp_buff, 11));
	printf("temp_buff: %s\n", temp_buff);
	print_buffer(buff);
	printf("\n\n");
	/*-----------------------------------------*/

	/*-----------------------------------------*/
	printf("\tCOPY AND DRAIN\n");
	memset(temp_buff, 0, 12);
	printf("BinaryBuffer_copy_and_drain(): %d\n",
			BinaryBuffer_copy_and_drain(buff, temp_buff, 6));
	printf("temp_buff: %s\n", temp_buff);
	print_buffer(buff);
	printf("\n\n");
	/*-----------------------------------------*/

	/*-----------------------------------------*/
	printf("\tSHRINK TO FIT\n");
	BinaryBuffer_shrink_to_fit(buff);
	print_buffer(buff);
	printf("\n\n");
	/*-----------------------------------------*/

	/*-----------------------------------------*/
	printf("\tRESIZE/CLEAR NO RESIZE\n");
	printf("BinaryBuffer_resize(): %d\n", BinaryBuffer_resize(buff, 128));
	BinaryBuffer_clear_no_resize(buff);
	print_buffer(buff);
	printf("\n\n");
	/*-----------------------------------------*/

	/*-----------------------------------------*/
	printf("\tCLEAR\n");
	BinaryBuffer_clear(buff);
	print_buffer(buff);
	printf("\n\n");
	/*-----------------------------------------*/

	/*-----------------------------------------*/
	printf("\tINSERT\n");
	BinaryBuffer_append(buff, "hello world", 11);
	print_buffer(buff);
	printf("BinaryBuffer_insert(): %d\n", BinaryBuffer_insert(buff, 5, " hello again", 12));
	print_buffer(buff);
	printf("\n\n");
	/*-----------------------------------------*/

	/*-----------------------------------------*/
	printf("\tCOPY BLOCK\n");
	BinaryBuffer_clear(buff);
	memset(temp_buff, 0, sizeof(temp_buff));
	BinaryBuffer_append(buff, "hello world", 11);
	BinaryBuffer_append(buff, "hello again", 11);
	print_buffer(buff);

	printf("BinaryBuffer_copy_block(): %d\n",
			BinaryBuffer_copy_block(buff, 6, 16, temp_buff, sizeof(temp_buff)));
	printf("temp_buff: %s\n", temp_buff);
	print_buffer(buff);
	printf("\n\n");
	/*-----------------------------------------*/

	/*-----------------------------------------*/
	printf("\tCOPY BLOCK AND DRAIN\n");
	memset(temp_buff, 0, sizeof(temp_buff));
	printf("BinaryBuffer_copy_block_and_drain(): %d\n",
			BinaryBuffer_copy_block_and_drain(buff, 6, 16, temp_buff, sizeof(temp_buff)));
	printf("temp_buff: %s\n", temp_buff);
	print_buffer(buff);
	printf("\n\n");
	/*-----------------------------------------*/

	/* Cleanup. */
	delBinaryBuffer(&buff);

	printf("Application closing!\n");
	return(0);
}

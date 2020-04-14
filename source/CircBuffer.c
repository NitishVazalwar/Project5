/*
 * CircBuffer.c
 *
 *  Created on: Apr 11, 2020
 *      Author: nitis
 */
#include <stdint.h>
#include <stdlib.h>
#include "CircBuffer.h"
#include "Uart0poll.h"
uint8_t Size(CircBuffer_t *buf)
{
	return buf->length;
}

CircBuffer_t* CircBufferCreate(void)
{
	return (CircBuffer_t*) malloc(sizeof(CircBuffer_t));
}

CBufferReturn_t CircularBufferInit(CircBuffer_t * buf, uint8_t size)
{
	/* Allocate memory */
	buf->circbuffer_start = (char*) malloc(sizeof(char) * size);

	/* Make sure memory is valid */
	if(!buf->circbuffer_start)
	{
		return FAIL;
	}

	/* Set buffer values */
	buf->head = buf->circbuffer_start;
	buf->tail = buf->circbuffer_start;
	buf->capacity = size;
	buf->length = 0;


	return SUCCESS;
}

CBufferReturn_t CircularBufferDestroy(CircBuffer_t * buf)
{
	free(buf->circbuffer_start);

	free(buf);

	return SUCCESS;
}

//uint16_t CBLengthData(CircBuffer_t *cb)
//{
//	return ((*cb->head – *cb->tail) & (*cb->capacity -1));// uses power of two assumption to
//													// determine length
//}
CBufferReturn_t	Initialized(CircBuffer_t * buf)
{
	/* Ensure the buffer pointers are all valid */
	if( buf->circbuffer_start && buf->head && buf->tail)
	{
		return SUCCESS;
	}

	else return FAIL;
}

CBufferReturn_t	CheckIfEmpty(CircBuffer_t * buf)
{
	if((buf->length == 0) && (buf->head == buf->tail))
		{
			return EMPTY;
		}
	else return SUCCESS;
}

CBufferReturn_t	CheckIfValid(CircBuffer_t * buf)
{
	if(buf->circbuffer_start) return SUCCESS;
	else return FAIL;
}




CBufferReturn_t CheckIfFull(CircBuffer_t * buf)
{
	if((buf->capacity==buf->length) && (buf->head==buf->tail))
	{
		return EMPTY;
	}
	else return SUCCESS;
}

CBufferReturn_t	CBAdd(CircBuffer_t * buf, char c)
{
	CBufferReturn_t ret;

	/* Check that the buffer is not full */
	if(CheckIfFull(buf) == FULL)
	{
		//Logger statement here

		ret = FULL;

//		if(REALLOCATE_BUFFER && (buf->numReallocs < 5))
//		{
//			ret = CircBufRealloc(buf);
//		}

		//else return ret;
	}

	//Critical Section starts here

	/* Add element by placing into current head position and moving head forward 1 or wrapping */
	*(buf->head) = c;
	(buf->head)++;
	(buf->length)++;


	char* bufend = (char*) buf->circbuffer_start + (sizeof(char) * buf->capacity);

	/* Check if it needs to be wrapped to the beginning */
	if(buf->head == bufend)
	{
		buf->head = buf->circbuffer_start;
	}

	//End of critical section
	return SUCCESS;
}

CBufferReturn_t CBRead(CircBuffer_t * buf, char *out)
{
	CBufferReturn_t ret;
	/*Check if not empty*/
	if(CheckIfEmpty(buf)==EMPTY)
	{
		//logger statement
		ret=EMPTY;
	}
	//Critical section starts
	*out= *(buf->tail);
	(buf->head)++;
	(buf->length)--;

	char* bufend = (char*) buf->circbuffer_start + (sizeof(char) * buf->capacity);

		/* Check if it needs to be wrapped to the beginning */
		if(buf->tail == bufend)
		{
			buf->tail = buf->circbuffer_start;
		}

		//Critical section ends

		return SUCCESS;
}








/*
 * CircBuffer.h
 *
 *  Created on: Apr 11, 2020
 *      Author: nitis
 */

#ifndef CIRCBUFFER_H_
#define CIRCBUFFER_H_
#include<stdint.h>
#define SIZE 16


typedef struct CircBuffer_t
{
	char* circbuffer_start;		// Beginning of allocated buffer
	char* head;				// Pointer modified with ADD operations
	char* tail;				// Pointer modified with REMOVE operations
	uint8_t length;			// The current length of the buffer
	uint32_t capacity;		// The character capacity of the buffer
	//uint8_t numReallocs;	// If realloc is enabled, how many times has it been reallocated
} CircBuffer_t;


typedef enum
{
	SUCCESS ,
	FULL ,
	EMPTY ,
	FAIL
} CBufferReturn_t;



CircBuffer_t* CircBufferCreate(void);
CBufferReturn_t CircularBufferInit(CircBuffer_t * buf, uint8_t size);
CBufferReturn_t CircularBufferDestroy(CircBuffer_t * buf);
CBufferReturn_t	Initialized(CircBuffer_t * buf);
CBufferReturn_t	CheckIfFull(CircBuffer_t * buf);
CBufferReturn_t	CheckIfEmpty(CircBuffer_t * buf);
CBufferReturn_t	CheckIfValid(CircBuffer_t * buf);
CBufferReturn_t	CBAdd(CircBuffer_t * buf, char c);
CBufferReturn_t	CBRead(CircBuffer_t * buf, char *out);
uint8_t Size(CircBuffer_t *buf);
#endif /* CIRCBUFFER_H_ */

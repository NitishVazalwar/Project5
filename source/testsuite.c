#include "System.h"
#include "uCUnit-v1.0.h"
#include "CircBuffer.h"
#include "testsuite.h"

void Test_CircBufferCreateAndInit(void)
{
	CircBuffer_t * Buffer;
	Buffer=CircBufferCreate();
	UCUNIT_TestcaseBegin("Create test");
	UCUNIT_CheckIsNotNull(Buffer);
	char * value=Buffer->circbuffer_start;
	UCUNIT_CheckIsNotNull(value);
	//init test
	CBufferReturn_t ret 	= CircularBufferInit(Buffer, TEST_SIZE);
	UCUNIT_CheckIsEqual(ret, SUCCESS);
	CircularBufferDestroy(Buffer);
	//failed test cases
	UCUNIT_CheckIsNull(Buffer);
	UCUNIT_CheckIsNull(value);


}

void Test_BasicBuffer(void)
    {
    	int i = 0;
    	char out;
    	char string[4] = {'a', 'b', 'c', 'd'};
    	CBufferReturn_t status;
    	CircBuffer_t * buffer = CircBufferCreate();
    	UCUNIT_TestcaseBegin("Basic Buffer Test");

    	/* Init buffer and ensure success */
    	status = CircularBufferInit(buffer, 4);
    	UCUNIT_CheckIsEqual(status, SUCCESS);
    	printf("\n\rstatus init %d",status);

    	/* Add characters to capacity */
    	for(i=0 ; i<4 ; i++)
    	{
    		status = CBAdd(buffer, string[i]);
    		UCUNIT_CheckIsEqual(status, SUCCESS);
    		printf("\n\rstatus Add %d",status);
    	}

    	/* Remove elements and ensure equality */
    	for(i=0; i<4; i++)
    	{
    		status = CBRead(buffer, &out);
    		UCUNIT_CheckIsEqual(status, SUCCESS);
    		UCUNIT_CheckIsEqual(string[i], out);
    		printf("\n\rstatus Read is %d",status);
    	}

    	CircularBufferDestroy(buffer);
    	UCUNIT_TestcaseEnd();
    }

void Test_Wrap(void)
{
	char string[5] = {'a', 'b', 'c', 'd', 'e'};
	char bufOut;
		int i;
		CBufferReturn_t bufStatus;
		CircBuffer_t * buffer = CircBufferCreate();
		UCUNIT_TestcaseBegin("Buffer Wrap Test");

		/* Init buffer and ensure success */
		bufStatus = CircularBufferInit(buffer, 4);
		UCUNIT_CheckIsEqual(bufStatus, SUCCESS);

		/* Add characters to capacity */
		for(i=0 ; i<4 ; i++)
		{
			bufStatus = CBAdd(buffer, string[i]);
			UCUNIT_CheckIsEqual(bufStatus, SUCCESS);
		}

		/* Take one element out of buffer */
		bufStatus = CBRead(buffer, &bufOut);
		UCUNIT_CheckIsEqual(bufStatus, SUCCESS);
		UCUNIT_CheckIsEqual(string[0], bufOut);

		/* Add another element to buffer - should wrap around to beginning */
		bufStatus = CBAdd(buffer, string[i]);

		bufStatus = SUCCESS;
		i = 0;
		/* Remove elements until buffer is empty */
		while(bufStatus == SUCCESS)
		{
			bufStatus = CBRead(buffer, &bufOut);
		}

		UCUNIT_CheckIsEqual(bufStatus, EMPTY);

		/* If Wrap Add test works, then if head and tail are equal then tail must have wrapped */
		UCUNIT_CheckIsEqual(buffer->head, buffer->tail);

		CircularBufferDestroy(buffer);
		UCUNIT_TestcaseEnd();


}

void RunTests()
{
	 Test_CircBufferCreateAndInit();
	Test_BasicBuffer();
	 Test_Wrap();

}

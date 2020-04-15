/*
 * Uart0poll.c
 *
 *  Created on: Apr 4, 2020
 *      Author: nitis
 */
#include "MKL25Z4.h"
#include <stdint.h>
#include <stdio.h>
#include "Uart0poll.h"
#include "CircBuffer.h"
#include "logger.h"
CircBuffer_t * RXBuffer;
CircBuffer_t * TXBuffer;
//Q_T TxQ, RxQ;
/*
Function to transmit a character assuming transmitter is available.
Function using these two functions to wait (block) for the transmitter to be available and then once available transmit a character and return
Function to check whether the receiver has a new character to receive
Function to receive the character assuming receiver has data
Function using these two functions to wait (block) for the receiver to receive a new character and then return that character
An echo function that uses the above functions to echo received characters one at a time back to the PC-based sender
*/

/*Function for UART hardware initialization including baud rate and serial message format (parity, start/stop bits)*/
void Init_UART0(uint32_t baud_rate)
{
	RXBuffer=CircBufferCreate();
		CBufferReturn_t ret 	= CircularBufferInit(RXBuffer, SIZE);
		if(ret != SUCCESS)
			{
				printf("Circular buffer RX failed");

				//logString(LL_Debug, FN_uartInit, "Creation of rx Buffer Failed\n\r\0");
				return;
			}
		TXBuffer=CircBufferCreate();
		ret=CircularBufferInit(TXBuffer, SIZE);
		if(ret != SUCCESS)
				{
					printf("Circular buffer TX failed");
					//logString(LL_Debug, FN_uartInit, "Creation of rx Buffer Failed\n\r\0");
					return;
				}

	uint16_t sbr;
	uint8_t temp;

	// Enable clock gating for UART0 and Port A
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

	// Make sure transmitter and receiver are disabled before init
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK;

	// Set UART clock to 48 MHz clock
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	SIM->SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;

	// Set pins to UART0 Rx and Tx
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Rx
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Tx

	// Set baud rate and oversampling ratio
	sbr = (uint16_t)((SYS_CLOCK)/(baud_rate * UART_OVERSAMPLE_RATE));
	UART0->BDH &= ~UART0_BDH_SBR_MASK;
	UART0->BDH |= UART0_BDH_SBR(sbr>>8);
	UART0->BDL = UART0_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);

	// Disable interrupts for RX active edge and LIN break detect, select one stop bit
	UART0->BDH |= UART0_BDH_RXEDGIE(0) | UART0_BDH_SBNS(0) | UART0_BDH_LBKDIE(0);

	// Don't enable loopback mode, use 8 data bit mode, don't use parity
	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(0) | UART0_C1_PE(0);
	// Don't invert transmit data, don't enable interrupts for errors
	UART0->C3 = UART0_C3_TXINV(0) | UART0_C3_ORIE(0)| UART0_C3_NEIE(0)
			| UART0_C3_FEIE(0) | UART0_C3_PEIE(0);

	// Clear error flags
	UART0->S1 = UART0_S1_OR(1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);

	// Try it a different way
//	UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK |
//									UART0_S1_FE_MASK | UART0_S1_PF_MASK;

	// Send LSB first, do not invert received data
	UART0->S2 = UART0_S2_MSBF(0) | UART0_S2_RXINV(0);

#if USE_UART_INTERRUPTS
	// Enable interrupts. Listing 8.11 on p. 234

	NVIC_SetPriority(UART0_IRQn, 2); // 0, 1, 2, or 3
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);

	// Enable receive interrupts but not transmit interrupts yet
	UART0->C2 |= UART_C2_RIE(1);
#endif

	// Enable UART receiver and transmitter
	UART0->C2 |= UART0_C2_RE(1) | UART0_C2_TE(1);

	// Clear the UART RDRF flag
	temp = UART0->D;
	UART0->S1 &= ~UART0_S1_RDRF_MASK;

}

uint8_t receive()
{
	return UART0->D;
}

void send(uint8_t str)
{
	UART0->D = str;
}

/*Function to check whether the transmitter is available to accept a new character for transmission*/

uart0return check_for_transmit()
{
	if(UART0->S1 & UART0_S1_TDRE_MASK)
	{
		//printf("\n\rReady to transmit");
		return READY;
	}
	else
	{
		//printf("\nNot ready for transmission");
		return NOTREADY;
	}
}

uart0return check_for_receive()
{
	if(!(UART0->S1 & UART0_S1_RDRF_MASK))
	{
	//	printf("\nReady to receive");
		return READY;
	}
	else
	{
	//	printf("\nNot ready for reception");
		return NOTREADY;
	}
}

void UART0_Transmit_Poll(char data)
{
		while (!(UART0->S1 & UART0_S1_TDRE_MASK))
			;
		send(data);

}



uint8_t UART0_Receive_Poll(void) {
		while (!(UART0->S1 & UART0_S1_RDRF_MASK))
			;
		receive();
}



void Send_String_Poll(uint8_t * str)
{
	// enqueue string
	while (*str != '\0') { // Send characters up to null terminator
		UART0_Transmit_Poll(*str++);
	}
}

void Echo_charblock(void)
{
#ifdef  POLL
	check_for_receive();
	char ch=UART0_Receive_Poll();
	check_for_transmit();
	UART0_Transmit_Poll(ch);
	Displaytime();
#endif

#ifdef USE_UART_INTERRUPTS
	Echo_INTR();

#endif

}

void application()
{
	char array[15];
		for(int i=0;i<=2;i++)
		{
			char ch=UART0_Receive_Poll();
			send(ch);
			CBAdd(TXBuffer, ch);
			ch=array[i];
			CBRead(RXBuffer, &ch);

		}
		for(int i=0;i<=2;i++)
		{
			CBRead(TXBuffer, &array[i]);
			send(array[i]);
		}


		//sort



}

void test(void)
{
	   char ch='z';
	    send(ch);
	    CBAdd(TXBuffer, ch);
	    CBRead(TXBuffer, &ch);
}
void applicationPoll()
{
	int array[15];
	int count[200]={0};
    for(int i=0;i<10;i++)
    {
    	int ch=UART0_Receive_Poll();
    	array[i]=ch;
    	count[ch]=count[ch]+1;

    }
    //sort
	for (int i = 0; i < 10; i++)                     //Loop for ascending ordering
	{
		for (int j = 0; j < 10; j++)             //Loop for comparing other values
		{
			if (array[j] > array[i])                //Comparing other array elements
			{
				int tmp = array[i];         //Using temporary variable for storing last value
				array[i] = array[j];            //replacing value
				array[j] = tmp;             //storing last value
			}
		}
	}
//  	for(int i = 0; i < 10; i++)
//  	{
//  		for(int j = i + 1; array[j] != '\0'; j++)
//  		{
//  			if(array[j] == array[i])
//			{
//  				for(int k = j; array[k] != '\0'; k++)
//				{
//					array[k] = array[k + 1];
//				}
// 			}
//		}
//	}

	for(int j=0;j<10;j++)
	{
		printf("%c",array[j]);
		int a=array[j];
		printf("%d\n\r",count[a]);
	}
//
//
//
//
//
//
}

uart0return SendCharacterNB(char c)
{
	CBufferReturn_t ret = CBAdd(TXBuffer, c);
	if(ret == 1)
	{
		Control_RGB_LEDs(1, 0, 0);
		return TX_FAIL;
	}

	else
		return TX_DONE;
}




uart0return ReadCharacterNB(char * c)
{
	CBufferReturn_t ret = CBRead(RXBuffer, c);
	if(ret == 2)
		return RX_FAIL;
	else
		return RX_DONE;
}

void Echo_INTR(void)
{


}


uart0return EchoNB(void)
{
	char data;

	//if(CheckIfEmpty(RXBuffer) != 2)
	{
		ReadCharacterNB(&data);
		SendCharacterNB(data);
		(UART0->C2 |= UART_C2_TIE_MASK);		// enable transmission
	}
	//if(CheckIfEmpty(TXBuffer) == 2)
	{
		(UART0->C2 &= ~UART_C2_TIE_MASK);	//stop transmitting if no more tx data available
	}

	return DONE;
}




// UART0 IRQ Handler. Listing 8.12 on p. 235
void UART0_IRQHandler(void)
{
#ifdef APP
	uint8_t ch;

	if (UART0->S1 & (UART_S1_OR_MASK |UART_S1_NF_MASK |
		UART_S1_FE_MASK | UART_S1_PF_MASK))
	{
			// clear the error flags
			UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK |
									UART0_S1_FE_MASK | UART0_S1_PF_MASK;
			// read the data register to clear RDRF
			ch = UART0->D;
	}
	if (UART0->S1 & UART0_S1_RDRF_MASK)
	{
		// received a character
		ch = UART0->D;
		if (!CheckIfFull(RXBuffer))
		{
			//Q_Enqueue(&RxQ, ch);
			CBAdd(RXBuffer, ch);
		}
		else
		{
			// error - queue full.
			// discard character
			printf("\nError");
		}
	}
	if ( (UART0->C2 & UART0_C2_TIE_MASK) && // transmitter interrupt enabled
			(UART0->S1 & UART0_S1_TDRE_MASK) )
	{ // tx buffer empty
		// can send another character
		if (!CheckIfEmpty(TXBuffer))
		{
			UART0->D = CBRead(TXBuffer, &ch);
		} else \
		{
			// queue is empty so disable transmitter interrupt
			UART0->C2 &= ~UART0_C2_TIE_MASK;
		}
	}
#endif

#ifdef ECHO
	if (UART0->S1 & UART0_S1_RDRF_MASK)
		{
			// received a character
			char ch = UART0->D;
			send(ch);
		}
	Displaytime();
#endif
}





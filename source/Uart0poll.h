/*
 * Uart0poll.h
 *
 *  Created on: Apr 4, 2020
 *      Author: nitis
 */

#ifndef UART0POLL_H_
#define UART0POLL_H_
#define UART_OVERSAMPLE_RATE 16
#define SYS_CLOCK 48e6
#include "queue.h"
#define POLL 2
//#define USE_UART_INTERRUPTS 1 // 0 for polled UART communications, 1 for interrupt-driven
#define ECHO 1
//#define APP 1
extern Q_T TxQ, RxQ;

#define critical_start() _disable_irq()
#define critical_end()   _enable_irq()
typedef enum
{
	READY,
	NOTREADY,
	TRANSMIT,
	RX_DONE,
	RX_FAIL,
	TX_DONE,
	TX_FAIL,
	DONE
}uart0return;
void Init_UART0(uint32_t baud_rate);
uint8_t check_for_transmit();
uint8_t check_for_receive();

void UART0_Transmit_Poll(char data);
uint8_t UART0_Receive_Poll(void);
void Send_String_Poll(uint8_t * str);
void Echo_charblock();
uart0return EchoNB(void);
void application();
void applicationPoll();
void test(void);
void Echo_INTR(void);
#endif /* UART0POLL_H_ */

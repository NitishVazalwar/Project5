/*
 * timer.c
 *
 *  Created on: Apr 11, 2020
 *      Author: nitis
 */

/*
 * timer.c
 *
 *  Created on: Mar 25, 2020
 *      Author: nitis
 */
#include "MKL25Z4.h"
#include <stdio.h>
#include "timer.h"

static uint16_t minutes=0;
static uint8_t seconds=0;
static uint8_t hours=0;
static uint8_t tenths=0;

void Init_SysTick(void)
{
	SysTick->LOAD = (48000000L/10);
	NVIC_SetPriority(SysTick_IRQn, 3);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	NVIC_EnableIRQ(SysTick_IRQn);

}

void SysTick_Handler(void)
{
	if(seconds==10)
	{
		tenths++;
	}

	if (seconds==60)
	{
		minutes++;
		seconds=0;
	}

	if (minutes==60)
	{
		hours++;
		minutes=0;
	}
}

void Displaytime()
{
	printf("\n\r%02d:%02d:%02d.%d",hours, minutes,seconds,tenths);
}




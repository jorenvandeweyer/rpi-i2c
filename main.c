/********************************************************************
Main file inspired by the armc-013.c file from the Raspberry-Pi Bare
Metal Tutorials by Brian Sidebotham and modified to implement an
interrupt-driven device driver for the Raspberry Pi 1 Model b+.
********************************************************************/

#include "rpi-interrupts.h" // other exception handlers
#include "rpi-gpio.h"
#include "rpi-uart.h"


void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
	gpio_init();
	set_GPIO_alterfunc(&gpio[14], 4);
	set_GPIO_alterfunc(&gpio[15], 4);

	RPI_GetIrqController()->Enable_IRQs_2 = RPI_IRQ_2_ARM_UART_IRQ;	

	/* Initialize uart and allow uart to send interrupts */
	uart_init();				// initialize UARTs

	/* Enable interrupts */
	_unlock();

	char line[128];
	while(1){
		uprintf("Enter a line from UARTS\n\r");
		ugets(&uart, line);
		uprintf("%s\n", line);
	}
}

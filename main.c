/********************************************************************
Main file inspired by the armc-013.c file from the Raspberry-Pi Bare
Metal Tutorials by Brian Sidebotham and modified to implement an
interrupt-driven device driver for the Raspberry Pi 1 Model b+.
********************************************************************/

#include "rpi-interrupts.h" // other exception handlers
#include "rpi-gpio.h"
#include "rpi-uart.h"
#include "rpi-i2c.h"

#define GY_521 0x68

const int TX_PIN = 14;
const int RX_PIN = 15;

const int SDA_pin = 2;
const int SCL_pin = 3;

void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
	gpio_init();
	set_GPIO_alterfunc(&gpio[TX_PIN], 4);
	set_GPIO_alterfunc(&gpio[RX_PIN], 4);

	set_GPIO_alterfunc(&gpio[SDA_pin], 4);
	set_GPIO_alterfunc(&gpio[SCL_pin], 4);

	RPI_GetIrqController()->Enable_IRQs_2 = RPI_IRQ_2_ARM_UART_IRQ;	

	/* Initialize uart and allow uart to send interrupts */
	uart_init();				// initialize UARTs

	/* Enable interrupts */
	_unlock();

	i2c_init();

	int *bytes = {0x3, 0x8, 0xF};

	write_bytes(GY_521, &bytes, 3);

	char line[128];
	while(1){
		uprintf("Enter a line from UARTS\n\r");
		ugets(&uart, line);
		uprintf("%s\n", line);
	}
}

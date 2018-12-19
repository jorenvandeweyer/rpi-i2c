/********************************************************************
Main file inspired by the armc-013.c file from the Raspberry-Pi Bare
Metal Tutorials by Brian Sidebotham and modified to implement an
interrupt-driven device driver for the Raspberry Pi 1 Model b+.
********************************************************************/

#include "rpi-interrupts.h" // other exception handlers
#include "rpi-gpio.h"
#include "rpi-uart.h"
#include "rpi-i2c.h"
#include "L3G4200D.h"
#include "rpi-timer.h"
#include <stdio.h>

const int TX_pin = 14;
const int RX_pin = 15;

const int SDA_pin = 2;
const int SCL_pin = 3;

void setup_L3G4200D() {
  uprintf("Initialize L3G4200D\n\r");
  uprintf("test\n\r");
  while(!L3G4200D_begin(L3G4200D_SCALE_2000DPS, L3G4200D_DATARATE_400HZ_50)) {
    uprintf("Could not find a valid L3G4200D sensor, check wiring!");
    delay_ms(200);
  }

  // Check selected scale
  uprintf("Selected scale: ");

  switch (L3G4200D_getScale()) {
    case L3G4200D_SCALE_250DPS:
      uprintf("250 dps");
      break;
    case L3G4200D_SCALE_500DPS:
      uprintf("500 dps");
      break;
    case L3G4200D_SCALE_2000DPS:
      uprintf("2000 dps");
      break;
    default:
      uprintf("unknown");
      break;
  };

  // Check Output Data Rate and Bandwidth
  uprintf("Output Data Rate: ");

  switch (L3G4200D_getOdrBw()) {
    case L3G4200D_DATARATE_800HZ_110:
      uprintf("800HZ, Cut-off 110");
      break;
    case L3G4200D_DATARATE_800HZ_50:
      uprintf("800HZ, Cut-off 50");
      break;
    case L3G4200D_DATARATE_800HZ_35:
      uprintf("800HZ, Cut-off 35");
      break;
    case L3G4200D_DATARATE_800HZ_30:
      uprintf("800HZ, Cut-off 30");
      break;
    case L3G4200D_DATARATE_400HZ_110:
      uprintf("400HZ, Cut-off 110");
      break;
    case L3G4200D_DATARATE_400HZ_50:
      uprintf("400HZ, Cut-off 50");
      break;
    case L3G4200D_DATARATE_400HZ_25:
      uprintf("400HZ, Cut-off 25");
      break;
    case L3G4200D_DATARATE_400HZ_20:
      uprintf("400HZ, Cut-off 20");
      break;
    case L3G4200D_DATARATE_200HZ_70:
      uprintf("200HZ, Cut-off 70");
      break;
    case L3G4200D_DATARATE_200HZ_50:
      uprintf("200HZ, Cut-off 50");
      break;
    case L3G4200D_DATARATE_200HZ_25:
      uprintf("200HZ, Cut-off 25");
      break;
    case L3G4200D_DATARATE_200HZ_12_5:
      uprintf("200HZ, Cut-off 12.5");
      break;
    case L3G4200D_DATARATE_100HZ_25:
      uprintf("100HZ, Cut-off 25");
      break;
    case L3G4200D_DATARATE_100HZ_12_5:
      uprintf("100HZ, Cut-off 12.5");
      break;
    default:
      uprintf("unknown");
      break;
  }

  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  L3G4200D_calibrate(100);

  // Set threshold sensivty. Default 3.
  // If you don't want use threshold, comment this line or set 0.
  L3G4200D_setThreshold(3);
}

// void floatToString(char* c, float number, int length) {
//   int dec = (int)number;
//   int pos = 0;
//   int len = 0;

//   itoa(c, dec);

//   len = strlen(c);
//   pos = len;

//   float dcp = number - (float)dec;

//   c[pos++] = ".";

//   while (dcp != 0 || pos < len+length) {
//     char d[1];
//     dcp *= 10;
//     int r = (int)dcp;
//     itoa(d, r);
//     c[pos++] = d;
//     dcp -= r;
//   }
// }

// void printFloat(float number) {
//   char c[50];
//   snprintf(c, number, 50);
//   uprintf(c);
// }

void loop_L3G4200D() {
  struct Vector raw = L3G4200D_readRaw();
  struct Vector norm = L3G4200D_readNormalize();

  uprintf("Xraw = %d", raw.x);
  uprintf(" Yraw = %d", raw.y);
  uprintf(" Zraw = %d", raw.z);

  uprintf(" Xnorm = %d", norm.x);
  uprintf(" Ynorm = %d", norm.y);
  uprintf(" ZNorm = %d\n\r", norm.z);
}

void kernel_main( unsigned int r0, unsigned int r1, unsigned int atags )
{
	gpio_init();

	set_GPIO_pullup(&gpio[SDA_pin]);
	set_GPIO_pullup(&gpio[SCL_pin]);

	set_GPIO_alterfunc(&gpio[TX_pin], 4);
	set_GPIO_alterfunc(&gpio[RX_pin], 4);
	
	set_GPIO_alterfunc(&gpio[SDA_pin], 4);
	set_GPIO_alterfunc(&gpio[SCL_pin], 4);

	RPI_GetIrqController()->Enable_IRQs_2 = RPI_IRQ_2_ARM_UART_IRQ;	

	/* Initialize uart and allow uart to send interrupts */
	uart_init();				// initialize UARTs

	/* Enable interrupts */
	_unlock();

	i2c_init();

	uprintf("\n\r\n\rSTARTING...\n\r");

	setup_L3G4200D();

	while (1) {
    loop_L3G4200D();
    delay_ms(200);
	}
}





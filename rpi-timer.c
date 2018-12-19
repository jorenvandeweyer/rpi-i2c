#include "rpi-timer.h"

rpi_system_timer * system_timer = (rpi_system_timer*) SYSTEM_TIMER_BASE;

void delay_us(unsigned int us) {
	volatile unsigned int ts = system_timer->CLO;

    while( ( system_timer->CLO - ts ) < us ) {
        /* BLANK */
    }
}

void delay_ms(unsigned int ms) {
    volatile unsigned int ts = system_timer->CLO;

    while ((system_timer->CLO - ts) < ms*1000); 
}

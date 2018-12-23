#include "rpi-timer.h"

rpi_system_timer * system_timer = (rpi_system_timer*) SYSTEM_TIMER_BASE;
void delay_us(unsigned int us) {
	volatile unsigned int ts = system_timer->CLO;

    while( ( system_timer->CLO - ts ) < us ) {
        //nothing
    }
}

void delay_ms(unsigned int ms) {
    volatile unsigned int ts = system_timer->CLO;

    while ((system_timer->CLO - ts) < ms*1000) {
        //nothing
    }
}

unsigned int get_time_us() {
    return system_timer->CLO;
}

unsigned int get_time_ms() {
    return (int)(system_timer->CLO/1000);
}

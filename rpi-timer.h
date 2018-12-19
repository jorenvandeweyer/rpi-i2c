#include "defines.h"

typedef struct {
    volatile unsigned int CS;	//System timer control/status
    volatile unsigned int CLO;	//System timer counter lower 32 bits
    volatile unsigned int CHI;	//System timer counter higher 32 bits
    volatile unsigned int C0;	//System timer compare 0
    volatile unsigned int C1;	//System timer compare 1
    volatile unsigned int C2;	//System timer compare 2
    volatile unsigned int C3;	//System timer compare 3
} rpi_system_timer;

void delay_us(unsigned int us);

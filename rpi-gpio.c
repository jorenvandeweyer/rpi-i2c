/********************************************************************
This c file is based on the original implementation of gpio.c
as described by K.C. Wang, <kwang@eecs.wsu.edu> in the book:
"Embedded and Real-Time Operation Systems" and modified for use 
on the Raspberry Pi 1 Model b+.

Additional comments/functions are added for a better explanation/
more functionality. 
********************************************************************/
#include "rpi-gpio.h"

void set_GPIO_pullup(GPIO *up) {
    gpio_reg[GPIO_GPPUD] = 0b10;
    int i;
    for (i = 0; i<150; i++);
    gpio_reg[GPIO_GPPUDCLK0] = (1 << up->gpio_bit);
    for (i = 0; i<150; i++);
    gpio_reg[GPIO_GPPUD] = 0;
    gpio_reg[GPIO_GPPUDCLK0] = (0 << up->gpio_bit);
}

void clear_GPIO_pullup_down() {
    gpio_reg[GPIO_GPPUD] = 0b00;
    int i;
    for (i = 0; i<150; i++);
    gpio_reg[GPIO_GPPUDCLK0] = -1;
    for (i = 0; i<150; i++);
    gpio_reg[GPIO_GPPUDCLK0] = 0;
    gpio_reg[GPIO_GPPUD] = 0b00;
}

/** @brief GPIO initialization. For each gpio the correct registers are stored 
		   in variables declaired in the gpio struct */
void gpio_init(){
	gpio_reg = (unsigned int*)GPIO_BASE;
	int i,j;
	int tiental;
	GPIO *gp;
	for(i = 0; i<=25;i++){
		j = i+2;
		gp = &gpio[j];
		gp->bit = (j%10)*3;
		gp->set = GPIO_GPSET0;
		gp->clr = GPIO_GPCLR0;
		gp->enable_falling_edge = GPIO_GPFEN0;
		gp->a_enable_falling_edge = GPIO_GPAFEN0;
		gp->detect_edge = GPIO_GPEDS0;
		gp->gpio_bit = j;
		
		tiental = j/10;
		switch (tiental){
			case 0:
				gp->sel = GPIO_GPFSEL0;
				break;
			case 1:
				gp->sel = GPIO_GPFSEL1;
				break;
			case 2:
				gp->sel = GPIO_GPFSEL2;
				break;
			default: break;
		}
	}

	clear_GPIO_pullup_down();
}

/** @brief Alter the gpio its functionality to an alternate function */
void set_GPIO_alterfunc(GPIO *up, int n){
	gpio_reg[up->sel] |= (n << up->bit);
}

/** @brief Set the gpio as an output */
void set_GPIO_output(GPIO *up){	
	gpio_reg[up->sel] |= (1 << up->bit);
}

/** @brief Set the gpio as an input */
void set_GPIO_input(GPIO *up){	
	gpio_reg[up->sel] &= ~( 7<< up->bit);
}

/** @brief Set gpio output HIGH */
void set_pin(GPIO *up){
	gpio_reg[up->set] = (1 << up->gpio_bit);
}

/** @brief Set gpio output LOW */
void clear_pin(GPIO *up){
	gpio_reg[up->clr] = (1 << up->gpio_bit);
}

/** @brief Help function to clear the edge detect event */
void clear_detect(GPIO *up){
	gpio_reg[up->detect_edge] = (1 << up->gpio_bit);
}

/** @brief Help function to enable the edge detect event*/
void enable_falling_edge(GPIO *up){
	gpio_reg[up->enable_falling_edge] |= (1 << up->gpio_bit);
	gpio_reg[up->a_enable_falling_edge] |= (1 << up->gpio_bit);
}

/** @brief Enable edge detect event */
void enable_edge_detect(GPIO *up){
	enable_falling_edge(up);
}

/** @brief  Return bit value for a specific register and corresponding bit number */
int getBit(int bitNumber, int reg){	
	int bit = (reg >> bitNumber);
	return bit &= 1;
}

/** @brief Edge detect event check */
int edge_detect(GPIO *up){
	if(getBit(up->gpio_bit, gpio_reg[up->detect_edge])){
		clear_detect(up);		
		return 1;
	}
	else{return 0;}
}





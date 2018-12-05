/********************************************************************
This header file is based on the original implementation of uart.c
as described by K.C. Wang, <kwang@eecs.wsu.edu> in the book:
"Embedded and Real-Time Operation Systems" and modified for use 
on the Raspberry Pi 1 Model b+.

Additional comments/functions are added for a better explanation/
more functionality. 
********************************************************************/
#ifndef RPI_UART_H
#define RPI_UART_H

#include "defines.h"

static char *tab = "0123456789ABCDEF";

/** @brief The UART variable set*/
typedef volatile struct uart{
  char *base;                               			// base address; as char *
  int n;                                    			// uart number 0-3
  char inbuf[SBUFSIZE];
  int indata, inroom, inhead, intail;
  char outbuf[SBUFSIZE];
  int outdata, outroom, outhead, outtail;
  volatile int txon;                        			// 1=TX interrupt is on
}UART;

UART uart;                               				// UART structure

extern int uart_init();
extern void uart_handler(UART *up);
extern int ugets(UART *up, char *s); 					// get a line from UART
extern int uprintf(char *fmt, ...);

static int do_rx(UART *up);                             // RX interrupt handler
static int do_tx(UART *up);                             // TX interrupt handler
static int ugetc(UART *up); 							// return a char from UART
static int uputc(UART *up, char c); 					// output a char to UART
static int uprints(UART *up, char *s); 					// output a string of chars
static int urpx(UART *up, int x);
static int uprintx(UART *up, int x);
static int urpu(UART *up, int x);
static int uprintu(UART *up, int x);
static int uprinti(UART *up, int x);
static int ufprintf(UART *up, char *fmt,...);

#endif

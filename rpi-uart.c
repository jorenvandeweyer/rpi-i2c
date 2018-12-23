/********************************************************************
This c file is based on the original implementation of uart.c
as described by K.C. Wang, <kwang@eecs.wsu.edu> in the book:
"Embedded and Real-Time Operation Systems" and modified for use 
on the Raspberry Pi 1 Model b+.

Additional comments/functions are added for a better explanation/
more functionality. 
********************************************************************/
#include "rpi-uart.h"
#include "rpi-gpio.h"

/** @brief UART initialization */
int uart_init()
{
  UART *up;
  up = &uart;
  up->base = (char *)(0x20201000); 
  *(up->base+CNTL) &= ~0x10;                  // disable UART FIFO
  *(up->base+IMSC) |= 0x30;
  up->n = 0;                                  // UART ID number
  up->indata = up->inhead = up->intail = 0;
  up->inroom = SBUFSIZE;
  up->outdata = up->outhead = up->outtail = 0;
  up->outroom = SBUFSIZE;
  up->txon = 0;
}

/** @brief UART handler called upon interrupt */
void uart_handler(UART *up)
{
  u8 mis = *(up->base + MIS); // read MIS register
  if (mis & (1<<4)) // MIS.bit4=RX interrupt
    do_rx(up);
  if (mis & (1<<5)) // MIS.bit5=TX interrupt
    do_tx(up);
}

/** @brief UART RX interrupt handler */
int do_rx(UART *up)                             
{
  char c;
  c = *(up->base+UDR);
  //printf("rx interrupt: %c\n", c);
  //if (c==0xD)
  //printf("\n");
  up->inbuf[up->inhead++] = c;
  up->inhead %= SBUFSIZE;
  up->indata++; up->inroom--;
}

/** @brief UART TX interrupt handler */
int do_tx(UART *up)
{
  char c;
  //printf("TX interrupt\n");
  if (up->outdata <= 0){ // if outbuf[ ] is empty
    *(up->base+IMSC) = 0x10; // disable TX interrupt
    up->txon = 0; // turn off txon flag
    return;
  }
  c = up->outbuf[up->outtail++];
  up->outtail %= SBUFSIZE;
  *(up->base+UDR) = (int)c; // write c to DR
  up->outdata--; up->outroom++;
} 

/** @brief Return a char from UART */
int ugetc(UART *up)
{
  char c;
  while(up->indata <= 0);                       // loop until up->data > 0 READONLY
  c = up->inbuf[up->intail++];
  up->intail %= SBUFSIZE;
  // updating variables: must disable interrupts
  _lock();
    up->indata--; up->inroom++;
  _unlock();
  return c;
}

/** @brief Output a char to UART */
int uputc(UART *up, char c)
{
  if (up->txon){                                //if TX is on, enter c into outbuf[]
    up->outbuf[up->outhead++] = c;
    up->outhead %= 128;
    _lock();
      up->outdata++; up->outroom--;
    _unlock();
    return;
  }
  // txon==0 means TX is off => output c & enable TX interrupt
  // PL011 TX is riggered only if write char, else no TX interrupt
  int i = *(up->base+UFR); // read FR
  while( *(up->base+UFR) & 0x20 );            // loop while FR=TXF
  *(up->base+UDR) = (int)c;                   // write c to DR
  *(up->base+IMSC) |= 0x30;                   // 0000 0000: bit5=TX mask bit4=RX mask
  up->txon = 1;
}

/** @brief Get a line from UART */
int ugets(UART *up, char *s) 
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s++);
  }
  *s = 0;
}

/** @brief Output a string of chars */
int uprints(UART *up, char *s) 					      // output a string of chars
{
	while (*s)
	 uputc(up, *s++);
}


/******************************************************************************/

int urpx(UART *up, int x)
{
  char c;
  if (x){
     c = tab[x % 16];
     urpx(up, x / 16);
  }
  uputc(up, c);
}

int uprintx(UART *up, int x)
{
  uprints(up, "0x");
  if (x==0)
    uputc(up, '0');
  else
    urpx(up, x);
  uputc(up, ' ');
}

int urpu(UART *up, int x) {
  char c;
  c = tab[x % 10];
  if (x > 9) {
    urpu(up, x / 10);
  }
  return uputc(up, c);
}

int uprintu(UART *up, int x)
{
  if (x==0)
    uputc(up, '0');
  else
    urpu(up, x);
}

int uprinti(UART *up, int x)
{
  if (x<0){
    uputc(up, '-');
    x = -x;
  }
  uprintu(up, x);
}

int ufprintf(UART *up, char *fmt,...)
{
  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt + 1;

  while(*cp){
    if (*cp != '%'){
      uputc(up, *cp);
      if (*cp=='\n')
	      uputc(up, '\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c': uputc(up, (char)*ip);      break;
    case 's': uprints(up, (char *)*ip);  break;
    case 'd': uprinti(up, *ip);           break;
    case 'u': uprintu(up, *ip);           break;
    case 'x': uprintx(up, *ip);  break;
    }
    cp++; ip++;
  }
}

int uprintf(char *fmt, ...) {
  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt + 1;

  UART *up = &uart;

  while(*cp) {
    if (*cp != '%') {
      uputc(up, *cp);
      if (*cp=='\n')
	      uputc(up, '\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c': uputc(up, (char)*ip);      break;
    case 's': uprints(up, (char *)*ip);   break;
    case 'd': uprinti(up, *ip);           break;
    case 'u': uprintu(up, *ip);           break;
    case 'x': uprintx(up, *ip);  break;
    }
    cp++; ip++;
  }
}

/******************************************************************************/

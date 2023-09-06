#include <MKL25Z4.H>
#include "mbed.h"

#define RS 0x04 /* PTA2 mask */
#define RW 0x10 /* PTA4 mask */
#define EN 0x20 /* PTA5 mask */



void delayMs(int n) {
    int i;
    SysTick->LOAD = 41940 - 1;
    SysTick->CTRL = 0x5; /* Enable the timer and choose sysclk as the clock
    source */
    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) /* wait until the COUNT flag is set */
        { }
    }
    SysTick->CTRL = 0;
    /* Stop the timer (Enable = 0) */
}
// main() runs in its own thread in the OS
int main()

{
void delayMs(int n);
int counter = 0;

SIM->SCGC5 |= 0x400; /* enable clock to Port B */
SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
PORTB->PCR[1] = 0x100; /* make PTB19 pin as GPIO */
PTB->PDDR |= 0x80000; /* make PTB19 as output pin */
PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
PTD->PDDR |= 0x02; /* make PTD1 as output pin */

    while (true) {
       /* use bit 0 of counter to control red LED */
        if (counter & 1) 

        /* turn on red LED */
        PTB->PCOR = 0x40000; 
        else
        PTB->PSOR = 0x40000; /* turn off red LED */

        /* use bit 1 of counter to control green LED */
        if (counter & 2) 
        /* turn on green LED */
        PTB->PCOR = 0x80000; 
        else
        PTB->PSOR = 0x80000; /* turn off green LED */

        /* use bit 2 of counter to control blue LED */
        if (counter & 4) 

        /* turn on blue LED */
        PTD->PCOR = 0x02;
        else
        PTD->PSOR = 0x02; /* turn off blue LED */

        counter++;

        delayMs(500); 

    }
}


/* PORTA interrupt from a switch */

/* Upon pressing a switch connecting either PTA1 or PTA2 to ground, the green LED will toggle for three times. */
/* Main program toggles red LED while waiting for interrupt from switches. */

#include <MKL25Z4.H>

void delayMs(int n);
int flag = 0;
int count = 0;
int main(void) {

    __disable_irq(); /* disable all IRQs */

    SIM->SCGC5 |= 0x400; /* enable clock to Port B */
    PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
    PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
    PTB->PDDR |= 0xC0000; /* make PTB18, 19 as output pin */
    PTB->PDOR |= 0xC0000; /* turn off LEDs */
    SIM->SCGC5 |= 0x200; /* enable clock to Port A */
    /* configure PTA1 for interrupt */
    PORTA->PCR[16] |= 0x00100; /* make it GPIO */
    PORTA->PCR[16] |= 0x00003; /* enable pull-up */
    PTA->PDDR &= ~0x10000; /* make pin input */
    PORTA->PCR[16] &= ~0xF0000; /* clear interrupt selection */
    PORTA->PCR[16] |= 0xA0000; /* enable falling edge INT */

    NVIC->ISER[0] |= 0x40000000; /* enable INT30 (bit 30 of ISER[0]) */
    __enable_irq(); /* global enable IRQs */

    while(1) {/* toggle the red LED continuously */
        if (flag == 0){
            PTB->PTOR |= 0x40000; /* toggle red LED */
            delayMs(500);
        }else if(flag == 1){
            for (int i = 0; i < 3; i++) {
                PTB->PDOR &= ~0x80000; /* turn on green LED */
                delayMs(500);
                PTB->PDOR |= 0x80000; /* turn off green LED */
                delayMs(500);
                
            }           
            flag = 0;
        }
    } 
}
/* A pushbutton switch is connecting either PTA1 or PTA2 to ground to trigger PORTA interrupt */
extern "C" void PORTA_IRQHandler(void) {
    flag = 1;
    /* toggle green LED (PTB19) three times */
    PORTA->ISFR = 0x00010000; /* clear interrupt flag */
}


void delayMs(int n) {
    int i;
    SysTick->LOAD = 41940 - 1;
    SysTick->CTRL = 0x5; /* Enable the timer and choose sysclk as the clock
    source */
    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) /* wait until the COUNT flag is set */
        { }
    }
    //SysTick->CTRL = 0;
    /* Stop the timer (Enable = 0) */
}
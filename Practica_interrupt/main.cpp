/* PORTA interrupt from a switch */

/* Upon pressing a switch connecting either PTA1 or PTA2 to ground, the green LED will toggle for three times. */
/* Main program toggles red LED while waiting for interrupt from switches. */

#include <MKL25Z4.H>
void delayMs(int n);
int main(void) {

__disable_irq(); /* disable all IRQs */
SIM->SCGC5 |= 0x400; /* enable clock to Port B */
PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
PTB->PDDR |= 0xC0000; /* make PTB18, 19 as output pin */
PTB->PDOR |= 0xC0000; /* turn off LEDs */
SIM->SCGC5 |= 0x200; /* enable clock to Port A */
/* configure PTA1 for interrupt */
PORTA->PCR[1] |= 0x00100; /* make it GPIO */
PORTA->PCR[1] |= 0x00003; /* enable pull-up */
PTA->PDDR &= ~0x0002; /* make pin input */
PORTA->PCR[1] &= ~0xF0000; /* clear interrupt selection */
PORTA->PCR[1] |= 0xA0000; /* enable falling edge INT */

/* configure PTA2 for interrupt*/
PORTA->PCR[2] |= 0x00100; /* make it GPIO */
PORTA->PCR[2] |= 0x00003; /* enable pull-up */
PTA->PDDR &= ~0x0004; /* make pin input */
PORTA->PCR[2] &= ~0xF0000; /* clear interrupt selection */
PORTA->PCR[2] |= 0xA0000; /* enable falling edge INT */
NVIC->ISER[0] |= 0x40000000; /* enable INT30 (bit 30 of ISER[0]) */
__enable_irq(); /* global enable IRQs */
while(1) {/* toggle the red LED continuously */
PTB->PTOR |= 0x40000; /* toggle red LED */
delayMs(500);} }
/* A pushbutton switch is connecting either PTA1 or PTA2 to ground to trigger PORTA interrupt */
extern "C" void PORTA_IRQHandler(void) {
int i;
/* toggle green LED (PTB19) three times */
for (i = 0; i < 3; i++) {
PTB->PDOR &= ~0x80000; /* turn on green LED */
delayMs(500);
PTB->PDOR |= 0x80000; /* turn off green LED */
delayMs(500);
}           
PORTA->ISFR = 0x00000006; /* clear interrupt flag */ }
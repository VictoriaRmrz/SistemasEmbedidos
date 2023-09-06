#include <MKL25Z4.h>
#include <math.h>
#include <stdio.h>

#define RS 0x04 /* PTA2 mask */
#define RW 0x10 /* PTA4 mask */
#define EN 0x20 /* PTA5 mask */

void ADC0_init(void);
void LED_set(int s);
void LED_init(void);
void delayMSTM(int n);

void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void); 

void delayMs(int n) {
    int i;
    SysTick->LOAD = 41940 - 1;
    SysTick->CTRL = 0x5; /* Enable the timer and choose sysclk as the clock source */

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0)
        /* wait until the COUNT flag is set */
        { }
    }
    SysTick->CTRL = 0; 
    /* Stop the timer (Enable = 0) */
/* turn on or off the LEDs wrt to bit 2-0 of the value */
}

void LCD_command(unsigned char command){
    PTA->PCOR = RS | RW; /* RS = 0, R/W = 0 */
    PTD->PDOR = command;
    PTA->PSOR = EN; /* pulse E */

    delayMs(0);
    PTA->PCOR = EN; 

    if (command < 4)
    delayMs(4); /* command 1 and 2 needs up to 1.64ms */
    else
    delayMs(1); /* all others 40 us */
}

/* Sends data to the LCD*/
void LCD_data(unsigned char data){
    PTA->PSOR = RS; /* RS = 1, R/W = 0 */
    PTA->PCOR = RW;
    PTD->PDOR = data;
    PTA->PSOR = EN; /* pulse E */
    delayMs(0);
    PTA->PCOR = EN;
    delayMs(1);
}

void LCD_init(void){
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    PORTD->PCR[0] = 0x100; /* make PTD0 pin as GPIO */
    PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
    PORTD->PCR[2] = 0x100; /* make PTD2 pin as GPIO */
    PORTD->PCR[3] = 0x100; /* make PTD3 pin as GPIO */
    PORTD->PCR[4] = 0x100; /* make PTD4 pin as GPIO */
    PORTD->PCR[5] = 0x100; /* make PTD5 pin as GPIO */
    PORTD->PCR[6] = 0x100; /* make PTD6 pin as GPIO */
    PORTD->PCR[7] = 0x100; /* make PTD7 pin as GPIO */
    PTD->PDDR = 0xFF; /* make PTD7-0 as output pins */
    SIM->SCGC5 |= 0x0200; /* enable clock to Port A */
    PORTA->PCR[2] = 0x100; /* make PTA2 pin as GPIO */
    PORTA->PCR[4] = 0x100; /* make PTA4 pin as GPIO */
    PORTA->PCR[5] = 0x100; /* make PTA5 pin as GPIO */
    PTA->PDDR |= 0x34; /* make PTA5, 4, 2 as out pins*/
    delayMs(30); /* initialization sequence */

    delayMs(30); /* initialization sequence */
    LCD_command(0x38);
    delayMs(1);
    LCD_command(0x01);
    /* set 8-bit data, 2-line, 5x7 font */
    LCD_command(0x38);
    /* move cursor right */
    LCD_command(0x06);
    /* clear screen, move cursor to home */
    LCD_command(0x01);
    /* turn on display, cursor blinking */
    LCD_command(0x0F);
}

int main (void)
{
short int result;
LED_init(); /* Configure LEDs */
ADC0_init(); /* Configure ADC0 */
while (1) {
ADC0->SC1[0] = 26; /* start conversion on channel 26 temperature */
while(!(ADC0->SC1[0] & 0x80)) { } /* wait for COCO */
result = ADC0->R[0]; /* read conversion result and clear COCO flag */
LED_set(result); /* display result on LED */
LCD_init();
LCD_command(1); // clear display
LCD_command(0x80); // set cursor at first line
int sizeT = (int)((ceil(log10(result))+1)*sizeof(char)); // size of string to represent time left
char num_str[sizeT];
sprintf(num_str, "%d", result); // set the time left to a string
for (int i=0; i<ceil(log10(result+1));i++){
    LCD_data('T'); 
    LCD_data('e');
    LCD_data('m');
    LCD_data('p');
    LCD_data('e');
    LCD_data('r');
    LCD_data('a');
    LCD_data('t');
    LCD_data('u');
    LCD_data('r');
    LCD_data('e');
    LCD_data(':');
    LCD_data(' ');
    LCD_data(num_str[i]);
    delayMSTM(500);
}
}
}
void ADC0_init(void)
{
SIM->SCGC6 |= 0x8000000; /* clock to ADC0 */
ADC0->SC2 &= ~0x40; /* software trigger */
/*CLKDIV/4, LS time, single ended 12 bit, bus clock */
ADC0->CFG1 = 0x40 | 0x10 | 0x04 | 0x00;
}

void LED_init(void) {
SIM->SCGC5 |= 0x400; /* enable clock to Port B */
SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
PTB->PDDR |= 0x80000; /* make PTB19 as output pin */
PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
PTD->PDDR |= 0x02; /* make PTD1 as output pin */
}


void LED_set(int s) {
if (s & 1) /* use bit 0 of s to control red LED */
PTB->PCOR = 0x40000; /* turn on red LED */
else
PTB->PSOR = 0x40000; /* turn off red LED */


if (s & 2) /* use bit 1 of s to control green LED */
PTB->PCOR = 0x80000; /* turn on green LED */
else
PTB->PSOR = 0x80000; /* turn off green LED */


if (s & 4) /* use bit 2 of s to control blue LED */
PTD->PCOR = 0x02; /* turn on blue LED */
else
PTD->PSOR = 0x02; /* turn off blue LED */
}


void delayMSTM(int n){
    int i;
    for (i = 0; i<n;i++){
        while((TPM0->SC & 0x80) == 0) { } // wait until the TOF is set
        TPM0->SC |= 0x80; // clear TOF
    }
}

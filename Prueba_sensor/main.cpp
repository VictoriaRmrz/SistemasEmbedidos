#include <MKL25Z4.H>
#include <LcdDisp.h>
#include <MklTime.h>

void sensor();
LcdDisp Lcd;

int main(void) {
    
    Lcd.lcdInit();
    SIM->SCGC5 |= 0x400; /* enable clock to Port B */
    SIM->SCGC5 |= 0x200; /* enable clock to Port A */
    PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
    PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
    PTB->PDDR |= 0xC0000; /* make PTB18, 19 as output pin */
    PTB->PDOR |= 0xC0000; /* turn off LEDs */
    /* configure PTA1 for interrupt */
    PORTA->PCR[12] = 0x102; /* make PTA1 GPIO & en pull-up */
    PTA->PDDR &= ~0x1000; /* make PTA1 as input pin */
    Lcd.lcdPrint("sensor");
    while(1) {
       
        PTB->PDOR &= ~0x40000; /* turn on red LED */
        sensor();
    } 
}

void sensor(){
    if (~(PTA->PDIR & 0x1000)){
        PTB->PCOR |= 0x40000; /* turn off red LED */
        PTB->PDOR |= 0x80000; /* turn on green LED */
        Lcd.lcdCommand(1); // clear display
        Lcd.lcdPrint("yes");
        delay_Ms(5000);
        //buzzer
    }else{
        PTB->PDOR &= ~0x80000; /* turn off green LED */
        Lcd.lcdCommand(1); // clear display
        Lcd.lcdPrint("no");
        delay_Ms(5000);
    }
}
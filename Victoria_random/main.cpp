//#include "mbed.h"
#include <MKL25Z4.h>
#include <cstdlib>
#include <time.h>
#include <LcdDisp.h>
#include <MklTime.h>

LcdDisp Lcd;

int n = 0;

void pwm(int n){
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    PORTD->PCR[1] = 0x0400; /* PTD1 used by TPM0 */
    SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
    SIM->SOPT2 |= 0x01000000; /* use MCGFLLCLK as timer counter clock */
    TPM0->SC = 0; /* disable timer */
    /* edge-aligned, pulse high */
    TPM0->CONTROLS[1].CnSC = 0x20 | 0x08; /* Set up modulo register for 60 kHz */
    TPM0->MOD = n;  //ESYR 656
    TPM0->CONTROLS[1].CnV = 656; 
    /* Set up channel value for % dutycycle */
    TPM0->SC = 0x0F; /* enable TPM0 with prescaler /16 */ //ESTE
    //while (1) { } 
    // PTE->PDOR &= ~0x1; /* turn off LED */
    PTD->PDOR |= 0x0400; /* send to buzzer */ 
    //PTD->PDOR |= 0x1; /* turn on LED */ 
    
}

int main (void) {
    Lcd.lcdInit();
    Lcd.lcdCommand(1); // clear display
    Lcd.lcdCommand(0x80); // set cursor at the first line
    Lcd.lcdPrint("Buzzer");

    //SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; /* enable clock to Port D */
    //PORTD->PCR[0] = 0x100; /* make PTD0 pin as GPIO */
    //PTD->PDDR |= 0x1; /* make PTD0 as output pin */
    

    n = 700;
    pwm(n);

    
    delay_Ms(2000);
    n = 0;
    pwm(n);
    //Lcd.lcdPrint("F");
    n = 458;
    pwm(n);
    n = 0;
    pwm(n);
    


}


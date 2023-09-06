#include "Buzzer.h"

void Buzzer::pwmInit(void){
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; /* enable clock to Port E */
    PORTD->PCR[5] = 0x100; /* make PTD5 pin as GPIO */
    PTD->PDDR |= 0x20; /* make PTD5 as output pin */
}

void Buzzer::pwm(int n){
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    PORTD->PCR[1] = 0x0400; /* PTD1 used by TPM0 */
    SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
    SIM->SOPT2 |= 0x01000000; /* use MCGFLLCLK as timer counter clock */
    TPM0->SC = 0; /* disable timer */
    /* edge-aligned, pulse high */
    TPM0->CONTROLS[1].CnSC = 0x20 | 0x08; 
    /* Set up modulo register for 60 kHz */
    TPM0->MOD = 43702; 
    TPM0->CONTROLS[1].CnV = n; 
    /* Set up channel value for % dutycycle */
    TPM0->SC = 0x0C; /* enable TPM0 with prescaler /16 */
    PTD->PDOR |= 0x20; /* send to buzzer */
}


void Buzzer::speed(int n){
     while(n!=0 && n>0){
        pwm(n);//0%
        n-=437;
        delay_Ms(500);
    }
}

int Buzzer::charToInt(char c){
    return c - '0';
}
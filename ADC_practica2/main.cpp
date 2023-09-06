#include <MKL25Z4.h>
#include <math.h>
#include <stdlib.h> // for the malloc function
#include <stdio.h>
#include <string.h>
#include <LcdDisp.h>
#include <KeyPad.h>

//#define RS 0x04 /* PTA2 mask */
//#define RW 0x10 /* PTA4 mask */
//#define EN 0x20 /* PTA5 mask */

LcdDisp Lcd;
KeyPad Keypad;

bool interrupt = false; //No hay botón presionado
float temperature; //Temperatura del sensor
unsigned char key;
int int_key;
extern "C" void PORTA_IRQHandler(void);
void ADC0_init(void);
//void imprimir(const char *a[], int n); //Función nueva que imprime a pantalla

int charToInt(char c){
    return c - '0';
}

int main() {
   
   //Varibales necesarias a usar
    char temp[] = "Temperature is: "; //Initial temperature
    char ask[] = "Desired temperature: ";
    char working[] = "Regulating..";
    char calor[] = "Warning! Excess!";
    char idle[] = "Below threshold.";
    bool exit = false;
    int tDeseada = 0;
    short int result;

    __disable_irq(); /* disable all IRQs */
    SIM->SCGC5 |= 0x400; /* enable clock to Port B */
    SIM->SCGC5 |= 0x200; /* enable clock to Port A */
    PORTB->PCR[0] = 0x100; /* make PTB0 pin as GPIO */
    PTB->PDDR |= 0x00100; /* make PTB0 as output pin */

    /* configure PTA16 for interrupt */
    PORTA->PCR[16] |= 0x00100; /* make it GPIO */
    PORTA->PCR[16] |= 0x00003; /* enable pull-up */
    PTA->PDDR &= ~0x10000; /* make pin input */
    PORTA->PCR[16] &= ~0xF0000; /* clear interrupt selection */

    /* configure PTA17 for interrupt*/
    PORTA->PCR[17] |= 0x00100; /* make it GPIO */
    PORTA->PCR[17] |= 0x00003; /* enable pull-up */
    PTA->PDDR &= ~0x20000; /* make pin input */
    PORTA->PCR[17] &= ~0xF0000; /* clear interrupt selection */
    
    //delay_init(); //Inicializing Delay
    delay_Ms(1000);
    ADC0_init(); /* Configure ADC0 */
    Lcd.lcdInit(); //Initializing LCD
    Keypad.init(); //Initializing Keypad
    
    NVIC->ISER[0] |= 0x40000000; /* enable INT30 (bit 30 of ISER[0]) */
    __enable_irq(); /* global enable IRQs */
   
    while(1){
        if (!interrupt){
            PORTA->PCR[16] &= ~(0xA0000); /* disable INT */
            PORTA->PCR[17] &= ~(0xA0000); /* disable INT */
            Lcd.lcdCommand(0x80); // set cursor at the first line
            Lcd.lcdPrint(ask);
            delay_Ms(1000);
            Lcd.lcdCommand(1); // clear display
            while (!exit) {    
                key = Keypad.getKey();
                int_key = charToInt(key);
                if (key == '*' || key == '#'){ // if pound or asterisk is pressed
                    exit = true;
                }  
                else if (int_key > 0 && int_key < 13 /*&& int_key % 4 != 0*/) { // if a number other than 0 is pressed
                    tDeseada *= 10;
                    tDeseada += int_key-int_key/4; // add number (compensating for keypad offset)
                   // Lcd.lcdData((int_key-int_key/4)+48); // display number 
                    Lcd.lcdData(key);
                } 
                else if (key == '0') { // if 0 is pressed
                    tDeseada *= 10;
                    Lcd.lcdData(48);
                }
                delay_Ms(300); // delay just long enough to prevent multiple digits being registered
            }
            interrupt = true;
            exit = false;
        }
        else {
            PORTA->PCR[16] |= 0xA0000; /* enable falling edge INT */
            PORTA->PCR[17] |= 0xA0000; /* enable falling edge INT */
            ADC0->SC1[0] = 0; /* start conversion on channel 13 temperature */
            //ADC0->SC1[0] = 0x20;
            while(!(ADC0->SC1[0] & 0x80)) { } /* wait for COCO */
            result = ADC0->R[0]; /* read conversion result and clear COCO flag */
            temperature = (result * 330.0 )/ 65536;
            Lcd.lcdCommand(1); // clear display
            Lcd.lcdCommand(0x80); // set cursor at the first line
            printf("%d\n",(int)temperature);

            //Convert temp to char
            char buffer [sizeof((int)temperature)*8+1];
            sprintf(buffer, "%d", (int)temperature);

            if (temperature >= tDeseada) {
                Lcd.lcdCommand(1); // clear display
                Lcd.lcdCommand(0x80); // set cursor at the first line
                Lcd.lcdPrint(calor);
                Lcd.lcdCommand(0xC0); //Salto de línea                                
                Lcd.lcdPrint(buffer);
                PTB->PDOR |= 0x00100; /* turn on FAN */
                delay_Ms(5000);
                PTB->PDOR &= ~0x00100; /* turn off FAN */
            } else if (temperature > tDeseada) {
                Lcd.lcdCommand(1); // clear display
                Lcd.lcdCommand(0x80); // set cursor at the first line
                Lcd.lcdPrint(working);
                Lcd.lcdCommand(0xC0); //Salto de línea
                Lcd.lcdPrint(buffer);
                PTB->PDOR |= 0x00100; /* turn on FAN */
                delay_Ms(1000);
                PTB->PDOR &= ~0x00100; /* turn off FAN */
            } else {
                Lcd.lcdCommand(1); // clear display
                Lcd.lcdCommand(0x80); // set cursor at the first line
                Lcd.lcdPrint(idle);
                Lcd.lcdCommand(0xC0); //Salto de línea
                Lcd.lcdPrint(buffer);
                delay_Ms(1000);
            }


        }
    }
}


void ADC0_init(void) {
    SIM->SCGC5 |= 0x2000; /* clock to PORTE */
    PORTE->PCR[20] = 0; /* PTE20 analog input */
    SIM->SCGC6 |= 0x8000000; /* clock to ADC0 */
    ADC0->SC2 &= ~0x40; /* software trigger */
    ADC0->SC3 |= 0x07; /* 32 samples average */
    /* clock div by 4, long sample time, single ended 16 bit, bus clock */
    ADC0->CFG1 = 0x40 | 0x10 | 0x0C | 0x00;
}


extern "C" void PORTA_IRQHandler(void) {
    int i;
    while (PORTA->ISFR & 0x00030000) {
        if (PORTA->ISFR & 0x00010000) {
            int sizeT = (int)((ceil(log10(abs((int)temperature)))+1)*sizeof(char)); // size of string to represent temperature
            char temp_str[sizeT+10]; 
            PTB->PDOR &= ~0x00100; /* turn off FAN */
            sprintf(temp_str, "Stopped:\n%d", (int)temperature);
            Lcd.lcdCommand(1); // clear display
            Lcd.lcdCommand(0x80); // set cursor at the first line
            Lcd.lcdPrint(temp_str);
            key = Keypad.getKey();
            while (key != '#') key = Keypad.getKey();
            PORTA->ISFR = 0x00030000; /* clear interrupt flag */ 
        }
        if (PORTA->ISFR & 0x00020000) {
            interrupt = false;
            PORTA->ISFR = 0x00030000; /* clear interrupt flag */ 
        } 
    }
}
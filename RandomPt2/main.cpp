#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <MKL25Z4.h>
#include <LcdDisp.h>

LcdDisp Lcd;


void ADC0_Init() {
    // Habilitar el reloj del ADC0
    SIM->SCGC5 |= 0x2000; /* clock to PORTE */
    PORTE->PCR[20] = 0; /* PTE20 analog input */
    SIM->SCGC6 |= 0x8000000; /* clock to ADC0 */
    ADC0->SC2 &= ~0x40; /* software trigger */
    ADC0->SC3 |= 0x07; /* 32 samples average */
    /* clock div by 4, long sample time, single ended 16 bit, bus clock */
    ADC0->CFG1 = 0x40 | 0x10 | 0x0C | 0x00;

    // Configurar el ADC0 para operación de un solo conversion
    ADC0->SC1[0] = 0; /* start conversion on channel 13 temperature */
}

int main() {
    // Inicializar la semilla para generar números aleatorios
    srand(time(NULL));
    delay_Ms(1000);
    ADC0_Init();
    //Lcd.lcdInit(); //Initializing LCD
    //Lcd.lcdPrint("Prueba: ");
    // Iniciar una nueva conversión
    ADC0->SC1[0] = 0;
    // Esperar a que la conversión se complete
     while(!(ADC0->SC1[0] & 0x80)) { }
    // Leer el valor del ADC0 y utilizarlo como semilla para el generador de números aleatorios
    short int valorADC = ADC0->R[0];
    srand(valorADC);
    //between 0 y 5
    int numeroAleatorio = rand()%5;
    //printf("Número aleatorio: %d\r\n", (int)numeroAleatorio);
    //Lcd.lcdData(numeroAleatorio + '0');

    return 0;
}
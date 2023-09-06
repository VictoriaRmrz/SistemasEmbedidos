#include <MKL25Z4.h>
#include <math.h>
#include <stdio.h>

#define RS 0x04 /* PTA2 mask */
#define RW 0x10 /* PTA4 mask */
#define EN 0x20 /* PTA5 mask */

void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void); 

/* initialize and configurates the LCD on the FRDM board */
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

/* This program scans a 4x4 matrix keypad and returns a unique number or each key pressed. 
The number is displayed on the tri-color
* LEDs using previous code
* PortC 7-4 are connected to the columns and PortC 3-0 are connected to the rows. */

void delayUs(int n);
void keypad_init(void);
char keypad_getkey(void);
void LED_init(void);

void delayUs(int n){
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
}

void LED_init(void){
    SIM->SCGC5 |= 0x400; /* enable clock to Port B */
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    PORTB->PCR[18] = 0x100; /* make PTB18 pin as GPIO */
    PTB->PDDR |= 0x40000; /* make PTB18 as output pin */
    PTB->PSOR |= 0x40000; /* turn off red LED */
    PORTB->PCR[19] = 0x100; /* make PTB19 pin as GPIO */
    PTB->PDDR |= 0x80000; /* make PTB19 as output pin */
    PTB->PSOR |= 0x80000; /* turn off green LED */
    PORTD->PCR[1] = 0x100; /* make PTD1 pin as GPIO */
    PTD->PDDR |= 0x02; /* make PTD1 as output pin */
    PTD->PSOR |= 0x02; /* turn off blue LED */
}

/* Initializes PortC that is connected to the keypad.
  Pins as GPIO input pin with pullup enabled.*/

void keypad_init(void){
    SIM->SCGC5 |= 0x0800;  /* enable clock to Port C */
    PORTC->PCR[0] = 0x103; /* PTD0, GPIO, enable pullup*/
    PORTC->PCR[1] = 0x103; /* PTD1, GPIO, enable pullup*/
    PORTC->PCR[2] = 0x103; /* PTD2, GPIO, enable pullup*/
    PORTC->PCR[3] = 0x103; /* PTD3, GPIO, enable pullup*/
    PORTC->PCR[4] = 0x103; /* PTD4, GPIO, enable pullup*/
    PORTC->PCR[5] = 0x103; /* PTD5, GPIO, enable pullup*/
    PORTC->PCR[6] = 0x103; /* PTD6, GPIO, enable pullup*/
    PORTC->PCR[7] = 0x103; /* PTD7, GPIO, enable pullup*/
    PTC->PDDR = 0x0F; /* make PTD7-0 as input pins */
}

/* keypad getkey()
* If a key is pressed, it returns a key code. Otherwise, a zero is returned.
The upper nibble of Port C is used as input. Pull-ups are enabled when the keys are not pressed
* The lower nibble of Port C is used as output that drives the keypad rows.
* First all rows are driven low and the input pins are read. If no key is pressed, it will read as all ones. Otherwise, some key is pressed. 
* If any key is pressed, the program drives one row low at a time and leave the rest of the rows inactive (float) then read the input pins.
* Knowing which row is active and which column is active, the program can decide which key is pressed. */

char keypad_getkey(void) {
    int row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08}; 
    /* one row is active */
    /* check to see any key pressed */

    PTC->PDDR |= 0x0F; /* enable all rows */
    PTC->PCOR = 0x0F;
    delayUs(2); /* wait for signal return */
    col = PTC-> PDIR & 0xF0; /* read all columns */
    PTC->PDDR = 0; /* disable all rows */
    if (col == 0xF0)
    return 0; /* no key pressed */

    /* If a key is pressed, we need to find out which key.*/ 
    for (row = 0; row < 4; row++){ 
        PTC->PDDR = 0; /* disable all rows */

        PTC->PDDR |= row_select[row]; /* enable one row */
        PTC->PCOR = row_select[row]; /* drive active row low*/

        delayUs(2); /* wait for signal to settle */
        col = PTC->PDIR & 0xF0; /* read all columns */

        if (col != 0xF0) break; 
        /* if one of the inputs is low, some key is pressed. */
    }
    
    PTC->PDDR = 0; /* disable all rows */
    if (row == 4) return 0; /* if we get here, no key is pressed */
    /* gets here when one of the rows has key pressed*/ 
    if (col == 0xE0) return row * 4 + 1; /* key in column 0 */
    if (col == 0xD0) return row * 4 + 2; /* key in column 1 */
    if (col == 0xB0) return row * 4 + 3; /* key in column 2 */
    if (col == 0x70) return row * 4 + 4; /* key in column 3 */
    return 0; /* just to be safe */
}

/* initialize all three LEDs on the FRDM board */

int main() {
    unsigned char key;
    SIM->SCGC5 |= 0x400; // enable clock to Port B
    PORTB->PCR[19] = 0x100; // make PTB19 pin as GPIO
    PTB->PDDR |= 0x080000; // make PTB19 as output pin
    PTB->PSOR |= 0x80000; // turn green LED off
    SIM->SCGC6 |= 0x01000000; // enable clock to TPM0
    SIM->SOPT2 |= 0x03000000; // use 32.768 kHz clock
    TPM0->SC = 0; // disable timer while configuring
    TPM0->SC = 0x02; // prescaler 4
    TPM0->MOD = 0x2000; // modulo 8192
    TPM0->SC |= 0x80; // clear TOF
    TPM0->SC |= 0x08; // enable timer free-running mode
    LCD_init(); //Initializing LCD
    keypad_init();
    LCD_command(1); // clear display
    LCD_command(0x80); // set cursor at first line
    LCD_data('H'); /* Writing hello */
    LCD_data('e');
    LCD_data('l');
    LCD_data('l');
    LCD_data('o');
    LCD_data(' ');
    LCD_data('T');
    LCD_data('i');
    LCD_data('m');
    LCD_data('e');
    LCD_data('r');
    LCD_data(' ');
    delayMs(5000);
    while (1) {
        bool exit = false;
        int time = 0;
        LCD_command(1); // clear display
        LCD_command(0x80); // set cursor at first line
        while (!exit) {
            key = keypad_getkey();
            if (key == 13 || key == 15) exit = true; // if pound or asterisk is pressed
            else if (key > 0 && key < 13 && key % 4 != 0) { // if a number other than 0 is pressed
                time *= 10;
                time += key-key/4; // add number (compensating for keypad offset)
                LCD_data((key-key/4)+48); // display number
            } else if (key == 14) { // if 0 is pressed
                time *= 10;
                LCD_data(48);
            }
            delayMs(300); // delay just long enough to prevent multiple digits being registered
        }
        while (key != 0) key = keypad_getkey(); // countdown starts a second after key is released
        delayMs(500);
        //time+=2; // to compensate for the lack of delay during the first iteration
        while (time > 0) {
            delayMs(200);
            int sizeT = (int)((ceil(log10(time))+1)*sizeof(char)); // size of string to represent time left
            char num_str[sizeT];
            delayMs(500); 
            LCD_command(0xC0); //Salto de línea
            sprintf(num_str, "%d", time); // set the time left to a string
            LCD_command(0x80); /* set cursor at first line */
            LCD_data('C');
            LCD_data('o');
            LCD_data('u');
            LCD_data('n');
            LCD_data('t');
            LCD_data('i');
            LCD_data('n');
            LCD_data('g');
            LCD_data('.');
            LCD_data('.');
            LCD_data('.');
            //We range in array of chars for print the differents elements
            for (int i=0; i<sizeT;i++){
                LCD_data(num_str[i]);
            }

            while((TPM0->SC & 0x80) == 0) { } // wait until the TOF is set
            TPM0->SC |= 0x80; // clear TOF
            time--;
        }
        LCD_command(1); /* clear display */
        LCD_command(0x80); /* set cursor at first line */
        LCD_data('F');
        LCD_data('i');
        LCD_data('n');
        LCD_data('i');
        LCD_data('s');
        LCD_data('h');
        LCD_data('e');
        LCD_data('d');
        LCD_data('.');
        PTD->PSOR = 0x02; // turn blue LED off
        PTB->PTOR = 0x080000; // toggle green LED when is ready
        delayMs(1000); // delay 1000 ms
        PTB->PTOR = 0x080000; // toggle green LED
    }
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

/* Delay n milliseconds */
/* The CPU core clock is set to MCGFLLCLK at */ /*41.94 MHz in SystemInit(). */

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

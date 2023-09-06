#include <MKL25Z4.h>
#include <LcdDisp.h>
#include <KeyPad.h>
#include <MklTime.h>

LcdDisp Lcd;
KeyPad Keypad;

void pwm(int n);
void input(void);
void manual(void);
void speed(int n);
int int_key;

int charToInt(char c){
    return c - '0';
}

int flag = 0;

int main(void){
    Lcd.lcdInit();
    Keypad.init();

    __disable_irq(); /* disable all IRQs */
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; /* enable clock to Port B */
    SIM->SCGC5 |= 0x200; /* enable clock to Port A */
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; /* enable clock to Port E */
    PORTE->PCR[1] = 0x100; /* make PTE1 pin as GPIO */
    PTE->PDDR |= 0x2; /* make PTE1 as output pin */
    PORTE->PCR[0] = 0x100; /* make PTE0 pin as GPIO */
    PTE->PDDR |= 0x1; /* make PTE0 as output pin */
    PORTB->PCR[0] = 0x100; /* make PTB0 pin as GPIO */
    PTB->PDDR |= 0x1; /* make PTB0 as output pin */
    
    /* configure PTA16 for interrupt */
    PORTA->PCR[16] |= 0x00100; /* make it GPIO */
    PORTA->PCR[16] |= 0x00003; /* enable pull-up */
    PTA->PDDR &= ~0x10000; /* make pin input */
    PORTA->PCR[16] &= ~0xF0000; /* clear interrupt selection */
    PORTA->PCR[16] |= 0xA0000; /* enable falling edge INT */

    /* configure PTA17 for interrupt*/
    PORTA->PCR[17] |= 0x00100; /* make it GPIO */
    PORTA->PCR[17] |= 0x00003; /* enable pull-up */
    PTA->PDDR &= ~0x20000; /* make pin input */
    PORTA->PCR[17] &= ~0xF0000; /* clear interrupt selection */
    PORTA->PCR[17] |= 0xA0000; /* enable falling edge INT */

    NVIC->ISER[0] |= 0x40000000; /* enable INT30 (bit 30 of ISER[0]) */
    __enable_irq(); /* global enable IRQs */

    while(1){
        if (flag == 0){   
            input();
            int n=0;
            while(n!=43703){
                pwm(n);//0%
                n+=437;
                delay_Ms(500);
            }
        }
        else if (flag == 1){
            Lcd.lcdPrint("Hello");
            manual();
            //PTB->PCOR = 0x40000;
            flag = 0;
        }
    }
}


void input(void){
    unsigned char key;
    bool key2=false;
    int time=0;
    char input[] = "1: M 2: A";
    Lcd.lcdPrint(input);
    
    while (!key2){
        key = Keypad.getKey();
        int_key = charToInt(key);
        if (key=='*' || key=='#' ){
            key2=true;
        }
        else if (int_key>0 && int_key<13){
            time = key;
            Lcd.lcdCommand(0xC0); //Salto de línea
            Lcd.lcdData(key);
        }
        else if (key=='0'){
            time*=10;
            Lcd.lcdCommand(0xC0); //Salto de línea
            Lcd.lcdData(48);
        }
        delay_Ms(300);
    }

    if (time == '1'){
        manual();
    }

    else{
        char error[] = "ERROR\nINVALID INPUT";
        Lcd.lcdCommand(1); // clear display
        Lcd.lcdCommand(0x80); // set cursor at the first line
        Lcd.lcdPrint(error);
    }
    key2=true;
}


void manual(void){
    unsigned char key;
    bool key2=false;
    int time=0;
    char select[30] = "SPEED";
    Lcd.lcdCommand(1);
    Lcd.lcdCommand(0x80);
    Lcd.lcdPrint(select);
    Lcd.lcdCommand(0xC0); //Salto de línea
    Lcd.lcdPrint("1:L 2:M 3:MH 4:H");
    delay_Ms(5000);
    
    
    while (!key2){
        key = Keypad.getKey();
        int_key = charToInt(key);
        if (key=='*' || key=='#'){
            key2=true;
        }
        else if (int_key>0 && int_key<13){
            time = key;
            //time*=10;
            //time+=(int_key-int_key/4);
            //LcdDisp.lcdData((key-key/4)+48);
            Lcd.lcdCommand(1); // clear display
            //Lcd.lcdCommand(0xC0); //Salto de línea
            Lcd.lcdData(key);
        }
        else if (key=='0'){
            time = key;
            //time*=10;
            Lcd.lcdCommand(1); // clear display
            //Lcd.lcdCommand(0xC0); //Salto de línea
            Lcd.lcdData(48);
        }
        delay_Ms(300);
    }
    if (time =='1'){//LOW 100%
        char low [] = "LOW" ;
        Lcd.lcdCommand(0xC0); //Salto de línea
        //Lcd.lcdCommand(1); // clear display
        Lcd.lcdCommand(0x80); // set cursor at the first line
        Lcd.lcdPrint(low);
        speed(43703);
        Lcd.lcdPrint("Finished.");
    }
    else if (time =='2'){//M 75%
        char medium [] ="MEDIUM";
        Lcd.lcdCommand(0xC0); //Salto de línea
        //Lcd.lcdCommand(1); // clear display
        Lcd.lcdCommand(0x80); // set cursor at the first line
        Lcd.lcdPrint(medium);
        speed(32775);
        Lcd.lcdPrint("Finished.");
    }
    else if (time =='3'){//MH 50%
        char mediumH[] = "MEDIUM-HIGH";
        Lcd.lcdCommand(0xC0); //Salto de línea
        //Lcd.lcdCommand(1); // clear display
        Lcd.lcdCommand(0x80); // set cursor at the first line
        Lcd.lcdPrint(mediumH);
        speed(21850);
        Lcd.lcdPrint("Finished.");
    }
    else if (time =='4'){//H 25%
        char high[] = "HIGH";
        //Lcd.lcdCommand(1); // clear display
        Lcd.lcdCommand(0xC0); //Salto de línea
        Lcd.lcdCommand(0x80); // set cursor at the first line
        Lcd.lcdPrint(high);
        speed(10925);
        Lcd.lcdPrint("Finished.");
    }
    else{
        char error[] = "ERROR ";
        Lcd.lcdCommand(1); // clear display
        //Lcd.lcdCommand(0xC0); //Salto de línea
        Lcd.lcdCommand(0x80); // set cursor at the first line
        Lcd.lcdPrint(error); 
        Lcd.lcdPrint("INVALID INPUT");
    }


}

void speed(int n){
     while(n!=0 && n>0){
        pwm(n);//0%
        n-=437;
        delay_Ms(500);
    }
    Lcd.lcdCommand(1); // clear display
    //Lcd.lcdPrint("F");
}


void pwm(int n){
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
    //while (1) { } 
    // PTE->PDOR &= ~0x1; /* turn off LED */
    PTE->PDOR |= 0x1; /* turn on LED */ 
    
}

extern "C" void PORTA_IRQHandler(void) {  
    flag = 0;
    while (PORTA->ISFR & 0x30000) {
        if (PORTA->ISFR & 0x20000) {
            flag = 1; 
            PORTA->ISFR = 0x20000; /* clear interrupt flag pt16- cafe */
        }
        if (PORTA->ISFR & 0x10000) {
            flag = 0;
            PORTA->ISFR = 0x10000; /* clear interrupt flag pt17- naranja*/
        } 
    } 
    PORTA->ISFR = 0x30000; /* clear interrupt flag */  
}

           

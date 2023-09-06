#include "KeyPad.h"

/**
 * @file KeyPad.cpp
 *
 * @brief Class for numeric matrix keypad initialization and key detection.
 */


KeyPad::KeyPad(){}

void KeyPad::rstRows(){
    PTB->PDDR &= ~ ( D1 | D2 | D3 | D4 );
}

void KeyPad::clrRows(){
    PTB->PCOR = ( D1 | D2 | D3 | D4 );
}

void KeyPad::setRows(){
    PTB->PDDR |= ( D1 | D2 | D3 | D4 );
}

void KeyPad::rstColumns(){
    PTE->PDDR &= ~( D5 | D6 | D7 | D8 );
}

void KeyPad::setColumns(){
    PTE->PDDR |= ( D5 | D6 | D7 | D8 );
}

/**
 * @brief Initializes the keypad by configuring the necessary pins and registers.
*/
void KeyPad::init(){

    SIM->SCGC5 |= ( SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK );
    PORTB->PCR[8] = 0x103; //Row (PTB8) D1
    PORTB->PCR[9] = 0x103; //Row (PTB9) D2
    PORTB->PCR[10] = 0x103; //Row (PTB10) D3
    PORTB->PCR[11] = 0x103; //Row (PTB11) D4
    PORTE->PCR[2] = 0x103; //Column (PTE2) D5
    PORTE->PCR[3] = 0x103; //Column (PTE3) D6
    PORTE->PCR[4] = 0x103; //Column (PTE4) D7
    PORTE->PCR[5] = 0x103; //Column (PTE5) D8

    //Make all rows as input
    setRows();

    //Make all columns output
    rstColumns();

}

/**
 * @brief Reads the pressed key from the keypad.
 *
 * @return The character corresponding to the pressed key. Returns '0' if no key is pressed.
*/
char KeyPad::getKey(){

    delay_Ms(2);
    int row, col;
    int rowNum;
    int colNum;

    /* one row is active */
    /* check to see any key pressed */

    setRows(); /* enable all rows */
    clrRows();

    delay_us(2); /* wait for signal return */

    col = (PTE->PDIR & (D5 | D6 | D7 | D8)) >> 2; /* read all columns */

    rstRows();
    rstColumns();

    if (col == 0xF)
    {
        return 0; /* No pressed */
    }

    int32_t rows[] = {D1,D2,D3,D4};
    /* If a key is pressed, we need find out which key.*/ 
    for (row = 0; row < 4; row++)
    {   
        rstRows();
        rstColumns();

        PTB->PDDR |= rows[row];
        PTB->PCOR = rows[row];

        delay_us(2); /* wait for signal to settle */
        col = (PTE->PDIR & (D5 | D6 | D7 | D8)) >> 2; /* read all columns */

        if (col != 0xF) break; 
        /* if one of the input is low, some key is pressed. */
    }
    rstRows();
    rstColumns();

    if (row == 4) return '0'; /* if we get here, no key is pressed */

    // Determine the key based on the column and row
    if (col == 0xE) return keypadValues[0][row]; /* key in column 0 */
    if (col == 0xD) return keypadValues[1][row]; /* key in column 1 */
    if (col == 0xB) return keypadValues[2][row]; /* key in column 2 */
    if (col == 0x7) return keypadValues[3][row]; /* key in column 3 */

    return 0; /* just to be safe */
}
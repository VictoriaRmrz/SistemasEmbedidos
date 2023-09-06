#include "LcdDisp.h"


LcdDisp::LcdDisp(){};

/**
 * @brief Pulses the Enable pin to latch the data.
 */
void LcdDisp::pulseEnable() {
    PTC->PDOR |= EN_PIN;
    delay_Ms(5);
    PTC->PDOR &= ~EN_PIN;
    delay_Ms(5);
}

/**
 * @brief Sends a command to the LCD.
 * @param command The command to send.
 */
void LcdDisp::lcdCommand(uint8_t command) {
    // Set RS pin LOW for command mode
    PTC->PDOR &= ~ (RS_PIN | RW_PIN); // RS = 0 (command mode)
    
    // Send higher nibble of command
    PTC->PDOR = ((command << 7) & 0x80) | ((command >> 1) & 0x1) | ((command << 1) & 0x78) | ((command << 4) & 0xC00);
    pulseEnable();
    
    if (command < 4)
    delay_Ms(10); /* command 1 and 2 needs up to 1.64ms */
    else
    delay_Ms(5); /* all others 40 us */

}

/**
 * @brief Sends a character to the LCD for display.
 * @param data The character to send.
 */
void LcdDisp::lcdData(unsigned char data) {
    PTC->PDOR |= RS_PIN; // RS = 1 (data mode)
    PTC->PDOR &= ~RW_PIN; // RW = 0

    // Send higher nibble of data
    PTC->PDOR = (PTC->PDOR & 0x12000) | (((data << 7) & 0x80) | ((data >> 1) & 0x1) | ((data << 1) & 0x78) | ((data << 4) & 0xC00));
    pulseEnable();

    delay_Ms(5);
}

/**
 * @brief Clears the LCD display.
 */
void LcdDisp::lcdClear() {
    lcdCommand(0x01); // Send clear display command
    delay_Ms(10);
    lcdCommand(0x80);   //Send cursor to first line
}

/**
 * @brief Prints a string on the LCD.
 * @param message The string to print.
 */
void LcdDisp::lcdPrint(const char* message) {
    while (*message != '\0') {
        lcdData(*message);
        message++;
    }
}

/**
 * @brief Sets the cursor position on the LCD.
 * @param row The row number (0-1).
 * @param column The column number (0-15).
 */
void LcdDisp::lcdSetCursor(int row, int column) {
    int row_offsets[] = {0x00, 0x40}; // Row offsets for a 20x4 LCD
    int offset = row_offsets[row] + column;
    lcdCommand(0x80 | offset); // Set cursor position command
}

/**
 * @brief Initializes the LCD display.
 */
void LcdDisp::lcdInit(){
    // Enable clock to Port C
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

    // Configure Port C pins for GPIO
    PORTC->PCR[7] = 0x100;   // Data pin 0 (PTC7)
    PORTC->PCR[0] = 0x100;   // Data pin 1 (PTC0)
    PORTC->PCR[3] = 0x100;   // Data pin 2 (PTC3)
    PORTC->PCR[4] = 0x100;   // Data pin 3 (PTC4)
    PORTC->PCR[5] = 0x100;   // Data pin 4 (PTC5)
    PORTC->PCR[6] = 0x100;   // Data pin 5 (PTC6)
    PORTC->PCR[10] = 0x100;  // Data pin 6 (PTC10)
    PORTC->PCR[11] = 0x100;  // Data pin 7 (PTC11)
    PORTC->PCR[12] = 0x100;  // Enable pin (PTC12)
    PORTC->PCR[13] = 0x100;  // Read/Write pin (PTC13)
    PORTC->PCR[16] = 0x100;  // Register Select pin (PTC16)

    // Set Port C pins as outputs
    PTC->PDDR |= EN_PIN | RW_PIN | RS_PIN | DATA_PIN_0 | DATA_PIN_1 | DATA_PIN_2 | DATA_PIN_3 | DATA_PIN_4 | DATA_PIN_5 | DATA_PIN_6 | DATA_PIN_7;

    delay_Ms(10); /* initialization sequence */
    lcdCommand(0x38);
    delay_Ms(5);
    lcdCommand(0x38);
    delay_Ms(1);
    lcdCommand(0x38);
    // Initialize LCD
    lcdCommand(0x38);   // 8-bit mode, 2 lines, 5x8 font
    lcdCommand(0x06);   // Entry mode set: increment cursor, no display shift
    lcdClear();         // Clear the display
    lcdCommand(0x0E);   // Display control: display on, cursor off, no blink
    lcdCommand(0x02);

}
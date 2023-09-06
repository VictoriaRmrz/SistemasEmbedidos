#include "UartCom.h"
#include <string>

Uart::Uart(){}

void Uart::init() {
    // Enable clock to UART0 module
    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
    
    // Configure UART0 pins
    PORTA->PCR[1] = PORT_PCR_MUX(2); // UART0_RX (PTA1)
    PORTA->PCR[2] = PORT_PCR_MUX(2); // UART0_TX (PTA2)
    
    // Disable transmitter and receiver while configuring UART0
    UART0->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    
    // Calculate UART0 baud rate divisor
    uint16_t sbr = (uint16_t)((SystemCoreClock / UART0_CLOCK_SELECT) / (UART0_BAUD_RATE * UART0_OVERSAMPLING_RATIO));
    
    // Set UART0 baud rate divisor
    UART0->BDH = (UART0->BDH & ~UART_BDH_SBR_MASK) | UART_BDH_SBR(sbr >> 8);
    UART0->BDL = (UART0->BDL & ~UART_BDL_SBR_MASK) | UART_BDL_SBR(sbr);
    
    // Enable transmitter and receiver
    UART0->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK;
}

void Uart::sendChar(char c) {
    // Wait until transmit data register is empty
    while (!(UART0->S1 & UART_S1_TDRE_MASK));
    
    // Send character
    UART0->D = c;
}

void Uart::sendString(const char* str) {
    while (*str) {
        sendChar(*str);
        str++;
    }
}

void Uart::sendString(std::string dataString) {
    const int length = dataString.length();
    char* char_array = new char[length + 1];
    strcpy(char_array, dataString.c_str());
    sendString(char_array);
    delete [] char_array;
}

void Uart::intToHex(uint32_t value, char* buffer, int bufferSize) {
    const char hexChars[] = "0123456789ABCDEF";
    int i = bufferSize - 1;
    
    buffer[i] = '\0';

    // Process each nibble of the value
    for (; i > 0 && value != 0; i--) {
        buffer[i - 1] = hexChars[value & 0x0F];
        value >>= 4;
    }

    // Add leading zeros if necessary
    while (i > 0) {
        buffer[i - 1] = '0';
        i--;
    }
}

void Uart::intToBinary(uint32_t value, char* buffer, int bufferSize) {
    int i = bufferSize - 1;

    buffer[i] = '\0';

    // Process each bit of the value
    for (; i > 0 && value != 0; i--) {
        buffer[i - 1] = (value & 1) ? '1' : '0';
        value >>= 1;
    }

    // Add leading zeros if necessary
    while (i > 0) {
        buffer[i - 1] = '0';
        i--;
    }
}

void Uart::sendCommand(uint32_t command) {
    // Convert the hexadecimal command to a string
    char hexCommand[9];
    intToHex(command, hexCommand, sizeof(hexCommand));
    
    // Send the command string
    sendString(hexCommand);
}

char Uart::readChar() {
    // Wait until receive data register is full
    while (!(UART0->S1 & UART_S1_RDRF_MASK));
    
    // Read and return the received character
    return UART0->D;
}

void Uart::readString(char* buffer, int bufferSize, char termination) {
    int i = 0;
    char c;
    
    while (i < bufferSize - 1) {
        c = readChar();
        
        if (c == termination) {
            break;
        }
        
        buffer[i] = c;
        i++;
    }
    
    buffer[i] = '\0';  // Null-terminate the string
}

void Uart::readString(char* stringVar) {
    int i = 0;
    char c = '\0';
    
    while (UART0->S1 & UART_S1_RDRF_MASK) {
        c = UART0->D;
        stringVar[i] = c;
        i++;
    }
    stringVar[i] = '\0';  // Null-terminate the string
}

void Uart::readString(char* stringVar, int bufferSize) {
    int i = 0;
    char c;

    while (UART0->S1 & UART_S1_RDRF_MASK) {
        c = UART0->D;
        // Lcd->lcdData(c);

        if (i < bufferSize - 1) {
            stringVar[i] = c;
            i++;
        }
    }

    // Null-terminate the string
    if (i < bufferSize) {
        stringVar[i] = '\0';
    } else {
        stringVar[bufferSize - 1] = '\0';
    }
}

bool Uart::available() {
    return (UART0->S1 & UART_S1_RDRF_MASK);
}



void Uart::readLine(char* buffer, int bufferSize) {
    int i = 0;
    char c;

    while (i < bufferSize - 1) {
        c = readChar();

        if (c == '\n' || c == '\r' || c == '\0' || c == '*') {
            buffer[i] = '\0';  // Null-terminate the string
            return;
        }

        buffer[i] = c;
        i++;
    }

    buffer[i] = '\0';  // Null-terminate the string
    sendString(buffer);
}

bool Uart::receivedFinishFlag(const std::string& message) {
    return (message == "end");
}
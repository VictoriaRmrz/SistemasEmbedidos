#ifndef UARTCOM_H
#define UARTCOM_H
#include <stdint.h>
#include <MKL25Z4.h>
#include <string>

#define UART_READ_TIMEOUT 1
class Uart{
    #define UART0_BAUD_RATE 9600
    #define UART0_CLOCK_SELECT 1
    #define UART0_OVERSAMPLING_RATIO 16

    public:
        Uart();
        void init();
        void sendChar(char c);
        void sendString(const char* str);
        void sendString(std::string dataString);
        void intToHex(uint32_t value, char* buffer, int bufferSize);
        void intToBinary(uint32_t value, char* buffer, int bufferSize);
        void sendCommand(uint32_t command);
        char readChar();
        void readString(char*, int, char);
        void readString(char*);
        void readString(char*, int); 
        bool available();
        void readLine(char*, int);
        bool receivedFinishFlag(const std::string& message);
    private:

};

#endif  // UARTCOM_H
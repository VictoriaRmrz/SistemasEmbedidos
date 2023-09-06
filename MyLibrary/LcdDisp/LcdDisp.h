#ifndef LCDDISP_H
#define LCDDISP_H

#include <MKL25Z4.h>
#include "MklTime.h"
#include <stdint.h>

class LcdDisp{

        #define EN_PIN   (1 << 12) // Enable pin (PTC12)
        #define RW_PIN   (1 << 13) // Read/Write pin (PTC13)
        #define RS_PIN   (1 << 16) // Register Select pin (PTC16)

        #define DATA_PIN_0  (1 << 7)  // Data pin 0 (PTC7)
        #define DATA_PIN_1  (1 << 0)  // Data pin 1 (PTC0)
        #define DATA_PIN_2  (1 << 3)  // Data pin 2 (PTC3)
        #define DATA_PIN_3  (1 << 4)  // Data pin 3 (PTC4)
        #define DATA_PIN_4  (1 << 5)  // Data pin 4 (PTC5)
        #define DATA_PIN_5  (1 << 6)  // Data pin 5 (PTC6)
        #define DATA_PIN_6  (1 << 10) // Data pin 6 (PTC10)
        #define DATA_PIN_7  (1 << 11) // Data pin 7 (PTC11)

    public:
        LcdDisp();
        void lcdInit();
        void lcdCommand(uint8_t command);
        void lcdData(unsigned char data);
        void lcdClear();
        void lcdPrint(const char* message);
        void lcdSetCursor(int row, int column);

    private:
        void pulseEnable();
};

#endif  // LCDDISP_H
#ifndef KEYPAD_H
#define KEYPAD_H

#include <MKL25Z4.h>
#include "MklTime.h"
#include "UartCom.h"
#include <stdint.h>

class KeyPad{
    char keypadValues[4][4] = {
        {'D', '#', '0', '*'},
        {'C', '9', '8', '7'},
        {'B', '6', '5', '4'},
        {'A', '3', '2', '1'}
    };
    int32_t D1 = (1<<8); //Row 1 
    int32_t D2 = (1<<9); //Row 2
    int32_t D3 = (1<<10); //Row 3
    int32_t D4 = (1<<11);  //Row 4
    int32_t D5 = (1<<2); //Column 1
    int32_t D6 = (1<<3); //Column 2
    int32_t D7 = (1<<4); //Column 3
    int32_t D8 = (1<<5); //Column 4
    
    private:
        void rstRows();
        void clrRows();
        void setRows();
        void rstColumns();
        void setColumns();
    
    public:
        KeyPad();
        void init();
        char getKey();
};

#endif // KEYPAD_H
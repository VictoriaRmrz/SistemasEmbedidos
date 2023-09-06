#ifndef BUZZER_H
#define BUZZER_H

#include <MKL25Z4.h>
#include "MklTime.h"
#include "UartCom.h"
#include <stdint.h>

class Buzzer{
    
    private:
    
    public:
        void pwmInit(void);
        void pwm(int n);
        void speed(int n);
        int charToInt(char c);
};

#endif 
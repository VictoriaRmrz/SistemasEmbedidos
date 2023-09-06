#include "MklTime.h"

/**
 * @file MklTime.h
 *
 * @brief Functions for delay timing using SysTick timer.
 */



/**
 * @brief Delay the execution for the specified number of microseconds.
 *
 * @param us The number of microseconds to delay.
 */
void delay_us(uint32_t us) {
    // Calculate the number of SysTick reload values required for the desired delay
    uint32_t ticks = us * (SystemCoreClock / 1000000);

    // Disable SysTick timer while configuring
    SysTick->CTRL = 0;

    // Set reload value
    SysTick->LOAD = ticks - 1;

    // Clear current value
    SysTick->VAL = 0;

    // Enable SysTick timer with processor clock as the source
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    // Wait until the count flag is set (indicates the desired delay has passed)
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));

    // Disable SysTick timer
    SysTick->CTRL = 0;
}

/**
 * @brief Delay the execution for the specified number of milliseconds.
 *
 * @param ms The number of milliseconds to delay.
 */
void delay_Ms(uint32_t ms) {
    // Calculate the number of SysTick reload values required for the desired delay
    uint32_t ticks = ms * (SystemCoreClock / 1000);

    // Disable SysTick timer while configuring
    SysTick->CTRL = 0;

    // Set reload value
    SysTick->LOAD = ticks - 1;

    // Clear current value
    SysTick->VAL = 0;

    // Enable SysTick timer with processor clock as the source
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    // Wait until the count flag is set (indicates the desired delay has passed)
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));

    // Disable SysTick timer
    SysTick->CTRL = 0;
}



/*First dalay test*/
// void delay_ms(int n) {
//     int i;
//     SysTick->LOAD = 41940 - 1;
//     SysTick->CTRL = 0x5; /* Enable the timer and choose sysclk as the clock
//     source */
//     for(i = 0; i < n; i++) {
//         while((SysTick->CTRL & 0x10000) == 0) /* wait until the COUNT flag is set */
//         { }
//     }
//     SysTick->CTRL = 0;
//     /* Stop the timer (Enable = 0) */
// }
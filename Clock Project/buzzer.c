#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C129.h>
#include "buzzer.h"

void Buzzer_Init(void) {
    SYSCTL->RCGCPWM |= (1 << 0);
    while ((SYSCTL->PRPWM & (1 << 0)) == 0) {
    }

    SYSCTL->RCGCGPIO |= (1 << 5);
    while ((SYSCTL->PRGPIO & (1 << 5)) == 0) {
    }

    GPIOF_AHB->AFSEL |= (1 << 1);
    GPIOF_AHB->PCTL = (GPIOF_AHB->PCTL & ~(0xF << 4)) | (6 << 4);
    GPIOF_AHB->DEN |= (1 << 1);

    PWM0->_0_CTL = 0;
    PWM0->_0_LOAD = 15999;
    PWM0->_0_CMPB = 8000;
    PWM0->_0_GENB = (2 << 10) | (3 << 2);
    PWM0->_0_CTL |= (1 << 0);

    PWM0->ENABLE &= ~(1 << 1);
}

void Buzzer_On(void) {
    PWM0->ENABLE |= (1 << 1);
}

void Buzzer_Off(void) {
    PWM0->ENABLE &= ~(1 << 1);
}

void Buzzer_AlarmTick(bool toneOn) {
    if (toneOn) {
        Buzzer_On();
    } else {
        Buzzer_Off();
    }
}
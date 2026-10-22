#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C129.h>
#include "rgb_led.h"

void RGB_Init(void) {
    SYSCTL->RCGCGPIO |= (1 << 5) | (1 << 6);
    while (((SYSCTL->PRGPIO) & ((1 << 5) | (1 << 6))) != ((1 << 5) | (1 << 6))) {
    }

    GPIOF_AHB->AFSEL &= ~((1 << 2) | (1 << 3));
    GPIOF_AHB->DIR |= (1 << 2) | (1 << 3);
    GPIOF_AHB->DEN |= (1 << 2) | (1 << 3);

    GPIOG_AHB->AFSEL &= ~(1 << 0);
    GPIOG_AHB->DIR |= (1 << 0);
    GPIOG_AHB->DEN |= (1 << 0);

    RGB_SetColor(LED_OFF);
}

void RGB_SetColor(LedColor_t color) {
    uint8_t rOn = 0, gOn = 0, bOn = 0;

    switch (color) {
        case LED_YELLOW:
            rOn = 1;
            gOn = 1;
            break;
        case LED_RED:
            rOn = 1;
            break;
        case LED_BLUE:
            bOn = 1;
            break;
        case LED_OFF:
        default:
            break;
    }

    if (rOn) {
        GPIOF_AHB->DATA |= (1 << 2);
    } else {
        GPIOF_AHB->DATA &= ~(1 << 2);
    }

    if (gOn) {
        GPIOF_AHB->DATA |= (1 << 3);
    } else {
        GPIOF_AHB->DATA &= ~(1 << 3);
    }

    if (bOn) {
        GPIOG_AHB->DATA |= (1 << 0);
    } else {
        GPIOG_AHB->DATA &= ~(1 << 0);
    }
}
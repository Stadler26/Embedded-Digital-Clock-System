#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C129.h>
#include "gpio_keypad.h"
#include "app_fsm.h"

volatile bool g_keyEventPending = false;

static const char keymap[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

void Keypad_Init(void) {
    SYSCTL->RCGCGPIO |= (1 << 10);
    while (((SYSCTL->PRGPIO) & (1 << 10)) == 0) {
    }
    GPIOL->AFSEL &= ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));
    GPIOL->DIR |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
    GPIOL->DEN |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
    GPIOL->DATA &= ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));

    SYSCTL->RCGCGPIO |= (1 << 7);
    while (((SYSCTL->PRGPIO) & (1 << 7)) == 0) {
    }
    GPIOH_AHB->AFSEL &= ~((1 << 2) | (1 << 3));
    GPIOH_AHB->DIR &= ~((1 << 2) | (1 << 3));
    GPIOH_AHB->DEN |= (1 << 2) | (1 << 3);
    GPIOH_AHB->PUR |= (1 << 2) | (1 << 3);
    GPIOH_AHB->IS &= ~((1 << 2) | (1 << 3));
    GPIOH_AHB->IBE &= ~((1 << 2) | (1 << 3));
    GPIOH_AHB->IEV &= ~((1 << 2) | (1 << 3));
    GPIOH_AHB->ICR |= (1 << 2) | (1 << 3);
    GPIOH_AHB->IM |= (1 << 2) | (1 << 3);

    SYSCTL->RCGCGPIO |= (1 << 12);
    while (((SYSCTL->PRGPIO) & (1 << 12)) == 0) {
    }
    GPION->AFSEL &= ~(1 << 2);
    GPION->DIR &= ~(1 << 2);
    GPION->DEN |= (1 << 2);
    GPION->PUR |= (1 << 2);
    GPION->IS &= ~(1 << 2);
    GPION->IBE &= ~(1 << 2);
    GPION->IEV &= ~(1 << 2);
    GPION->ICR |= (1 << 2);
    GPION->IM |= (1 << 2);

    NVIC->IPR[32] = 1;
    NVIC->ISER[1] |= (1 << 0);

    NVIC->IPR[73] = 1;
    NVIC->ISER[2] |= (1 << 9);
}


void GPIOH_Handler(void) {
    GPIOH_AHB->ICR |= (1 << 2) | (1 << 3);
    GPIOH_AHB->IM &= ~((1 << 2) | (1 << 3));
    GPION->IM &= ~(1 << 2);
    g_keyEventPending = true;
}

void GPION_Handler(void) {
    GPION->ICR |= (1 << 2);
    GPIOH_AHB->IM &= ~((1 << 2) | (1 << 3));
    GPION->IM &= ~(1 << 2);
    g_keyEventPending = true;
}

void Keypad_Service(void) {
    uint8_t row;
    uint8_t col1, col2, col3;
    uint8_t rowIndex = 0xFF;
    uint8_t colIndex = 0xFF;

    for (row = 0; row < 4; row++) {
        GPIOL->DATA = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
        GPIOL->DATA &= ~(1u << row);

        col1 = (GPIOH_AHB->DATA >> 2) & 1;
        col2 = (GPIOH_AHB->DATA >> 3) & 1;
        col3 = (GPION->DATA >> 2) & 1;

        if (col1 == 0 || col2 == 0 || col3 == 0) {
            rowIndex = row;
            if (col1 == 0) {
                colIndex = 0;
            } else if (col2 == 0) {
                colIndex = 1;
            } else {
                colIndex = 2;
            }
            break;
        }
    }

    GPIOL->DATA &= ~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3));
    g_keyEventPending = false;

    GPIOH_AHB->ICR |= (1 << 2) | (1 << 3);
    GPION->ICR |= (1 << 2);
    GPIOH_AHB->IM |= (1 << 2) | (1 << 3);
    GPION->IM |= (1 << 2);

    if (rowIndex != 0xFF && colIndex != 0xFF) {
        FSM_HandleKey((uint8_t)keymap[rowIndex][colIndex]);
    }
}
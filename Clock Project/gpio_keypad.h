#ifndef GPIO_KEYPAD_H
#define GPIO_KEYPAD_H

#include <stdint.h>
#include <stdbool.h>

extern volatile bool g_keyEventPending;

void Keypad_Init(void);
void Keypad_Service(void);
void GPIOH_Handler(void);
void GPION_Handler(void);

#endif
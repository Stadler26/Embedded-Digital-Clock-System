#ifndef ADC_POT_H
#define ADC_POT_H

#include <stdint.h>
#include <stdbool.h>

extern volatile uint16_t g_potValue;
extern volatile bool g_potReady;

void ADC_Init(void);
void ADC_StartConversion(void);
void ADC0SS3_Handler(void);

#endif
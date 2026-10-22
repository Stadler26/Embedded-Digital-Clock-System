#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C129.h>
#include "adc_pot.h"

volatile uint16_t g_potValue = 0;
volatile bool g_potReady = false;

static void Delay(volatile uint32_t count) {
    while (count--) {
    }
}

void ADC0SS3_Handler(void) {
    g_potValue = (uint16_t)(ADC0->SSFIFO3 & 0xFFF);
    ADC0->ISC = (1 << 3);
    g_potReady = true;
}

void ADC_Init(void) {
    SYSCTL->RCGCGPIO |= (1 << 4);
    while ((SYSCTL->PRGPIO & (1 << 4)) == 0) {
    }
    GPIOE_AHB->AFSEL |= (1 << 0);
    GPIOE_AHB->DEN &= ~(1 << 0);
    GPIOE_AHB->DIR &= ~(1 << 0);
    GPIOE_AHB->AMSEL |= (1 << 0);

    SYSCTL->RCGCADC |= (1 << 0);
    while ((SYSCTL->PRADC & (1 << 0)) == 0) {
    }

    Delay(10000);

    SYSCTL->PLLFREQ0 |= (1 << 23);
    while ((SYSCTL->PLLSTAT & 1) == 0) {
    }

    SYSCTL->ALTCLKCFG = 0;

    ADC0->CC = 0x1;

    SYSCTL->PLLFREQ0 &= ~(1 << 23);

    Delay(10000);

    ADC0->ACTSS &= ~(1 << 3);
    ADC0->EMUX &= ~(0xF << 12);
    ADC0->SSEMUX3 = 0;
    ADC0->SSMUX3 = 3;
    ADC0->SSCTL3 = 0x6;
    ADC0->ISC = (1 << 3);
    ADC0->IM |= (1 << 3);
    ADC0->ACTSS |= (1 << 3);

    NVIC->ISER[0] |= (1 << 17);
}

void ADC_StartConversion(void) {
    ADC0->PSSI = (1 << 3);
}
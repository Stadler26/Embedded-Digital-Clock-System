#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C129.h>
#include "uart_display.h"
#include "app_fsm.h"
#include "snooze.h"

#define UART_TX_BUF_SIZE 128

static volatile char txBuffer[UART_TX_BUF_SIZE];
static volatile uint16_t txHead = 0;
static volatile uint16_t txTail = 0;

void UART_Init(void) {
    SYSCTL->RCGCUART |= (1 << 0);
    while (((SYSCTL->PRUART) & (1 << 0)) == 0) {
    }

    SYSCTL->RCGCGPIO |= (1 << 0);
    while (((SYSCTL->PRGPIO) & (1 << 0)) == 0) {
    }
    GPIOA_AHB->AFSEL |= (1 << 0) | (1 << 1);
    GPIOA_AHB->PCTL = (GPIOA_AHB->PCTL & ~((1 << 8) - 1)) | ((1 << 0) | (1 << 4));
    GPIOA_AHB->DEN |= (1 << 0) | (1 << 1);

    UART0->CTL &= ~(1 << 0);
    UART0->IBRD = 104;
    UART0->FBRD = 11;
    UART0->LCRH = (1 << 4) | (1 << 5) | (1 << 6);
    UART0->CC = 0;
    UART0->IFLS = 0;
    UART0->IM = 0;
    UART0->ICR = 0xFFFF;

    NVIC->IPR[5] = 4;
    NVIC->ISER[0] |= (1 << 5);

    UART0->CTL |= (1 << 0) | (1 << 8) | (1 << 9);

    UART_WriteString("\x1B[2J\x1B[H");
}

void UART_WriteChar(char c) {
    uint16_t nextHead = (txHead + 1) % UART_TX_BUF_SIZE;
    while (nextHead == txTail) {
    }

    if (txHead == txTail && !((UART0->FR) & (1 << 5))) {
        UART0->DR = c;
    } else {
        txBuffer[txHead] = c;
        txHead = nextHead;
    }

    UART0->IM |= (1 << 5);
}

void UART_WriteString(const char *str) {
    while (*str) {
        UART_WriteChar(*str);
        str++;
    }
}

void UART0_Handler(void) {
    UART0->ICR |= (1 << 5);
    while (txTail != txHead && !((UART0->FR) & (1 << 5))) {
        UART0->DR = txBuffer[txTail];
        txTail = (txTail + 1) % UART_TX_BUF_SIZE;
    }
    if (txTail == txHead) {
        UART0->IM &= ~(1 << 5);
    }
}

static void AppendTwoDigit(char *buf, uint8_t *idx, uint8_t value) {
    buf[(*idx)++] = (char)('0' + (value / 10));
    buf[(*idx)++] = (char)('0' + (value % 10));
}

void Display_Render(void) {
    char line[64];
    uint8_t idx = 0;
    uint8_t displayHour;
    bool showColon;
    bool isPM;
    const volatile TimeRecord_t *shown;

    shown = (g_systemMode == MODE_SET_ALARM) ? &g_alarmTime : &g_currentTime;

    displayHour = shown->hours;
    isPM = (displayHour >= 12);
    if (!g_currentTime.is24Hour) {
        if (displayHour == 0) {
            displayHour = 12;
        } else if (displayHour > 12) {
            displayHour -= 12;
        }
    }

    showColon = g_secondsFlashOn &&
                (g_systemMode == MODE_NORMAL || g_systemMode == MODE_ALARM_SOUND);

    line[idx++] = 0x1B;
    line[idx++] = '[';
    line[idx++] = 'H';

    AppendTwoDigit(line, &idx, displayHour);
    line[idx++] = showColon ? ':' : ' ';
    AppendTwoDigit(line, &idx, shown->minutes);
    line[idx++] = showColon ? ':' : ' ';
    AppendTwoDigit(line, &idx, shown->seconds);
    line[idx++] = ' ';

    if (!g_currentTime.is24Hour) {
        line[idx++] = isPM ? 'P' : 'A';
        line[idx++] = 'M';
    }

    line[idx++] = ' ';
    line[idx++] = ' ';

    switch (g_systemMode) {
        case MODE_SET_TIME:
            line[idx++] = 'S';
            line[idx++] = 'E';
            line[idx++] = 'T';
            line[idx++] = 'T';
            line[idx++] = 'I';
            line[idx++] = 'M';
            line[idx++] = 'E';
            break;
        case MODE_SET_ALARM:
            line[idx++] = 'S';
            line[idx++] = 'E';
            line[idx++] = 'T';
            line[idx++] = 'A';
            line[idx++] = 'L';
            line[idx++] = 'A';
            line[idx++] = 'R';
            line[idx++] = 'M';
            break;
        case MODE_ALARM_SOUND:
            line[idx++] = 'A';
            line[idx++] = 'L';
            line[idx++] = 'A';
            line[idx++] = 'R';
            line[idx++] = 'M';
            break;
        default:
            if (g_snoozeActive) {
                line[idx++] = 'S';
                line[idx++] = 'N';
                line[idx++] = 'O';
                line[idx++] = 'O';
                line[idx++] = 'Z';
                line[idx++] = 'E';
            } else {
                line[idx++] = 'N';
                line[idx++] = 'O';
                line[idx++] = 'R';
                line[idx++] = 'M';
                line[idx++] = 'A';
                line[idx++] = 'L';
            }
            break;
    }

    line[idx++] = ' ';
    line[idx++] = ' ';
    line[idx++] = ' ';
    line[idx++] = ' ';
    line[idx++] = '\r';
    line[idx++] = '\n';
    line[idx] = '\0';

    UART_WriteString(line);
}
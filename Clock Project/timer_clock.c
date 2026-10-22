#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C129.h>
#include "timer_clock.h"
#include "app_fsm.h"
#include "rgb_led.h"
#include "buzzer.h"
#include "snooze.h"

void Timer_Init(void) {
    SYSCTL->RCGCTIMER |= (1 << 0);
    while (((SYSCTL->PRTIMER) & (1 << 0)) == 0) {
    }

    TIMER0->CTL &= ~(1 << 0);
    TIMER0->CFG = 0;
    TIMER0->TAMR = (1 << 1);
    TIMER0->TAILR = 15999999;
    TIMER0->ICR |= (1 << 0);
    TIMER0->IMR |= (1 << 0);

    NVIC->IPR[19] = 2;
    NVIC->ISER[0] |= (1 << 19);

    TIMER0->CTL |= (1 << 0);
}

void TIMER0A_Handler(void) {
    TIMER0->ICR |= (1 << 0);

    g_secondsFlashOn = !g_secondsFlashOn;
    g_currentTime.seconds++;

    if (g_currentTime.seconds >= 60) {
        g_currentTime.seconds = 0;
        g_currentTime.minutes++;
        if (g_currentTime.minutes >= 60) {
            g_currentTime.minutes = 0;
            g_currentTime.hours++;
            if (g_currentTime.hours >= 24) {
                g_currentTime.hours = 0;
            }
        }
    }

    g_currentTime.isPM = (g_currentTime.hours >= 12);
    g_displayDirty = true;

    if (g_systemMode == MODE_ALARM_SOUND) {
        RGB_SetColor(g_secondsFlashOn ? LED_RED : LED_BLUE);
        Buzzer_AlarmTick(g_secondsFlashOn);
    } else if (g_alarmArmed &&
               g_currentTime.hours == g_alarmTime.hours &&
               g_currentTime.minutes == g_alarmTime.minutes &&
               g_currentTime.seconds == 0) {
        g_systemMode = MODE_ALARM_SOUND;
        Buzzer_On();
    } else if (Snooze_CheckDue(g_currentTime.hours, g_currentTime.minutes, g_currentTime.seconds)) {
        g_systemMode = MODE_ALARM_SOUND;
        Buzzer_On();
    }
}
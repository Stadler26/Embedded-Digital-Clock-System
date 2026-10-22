#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C129.h>
#include "app_fsm.h"
#include "adc_pot.h"
#include "rgb_led.h"
#include "buzzer.h"
#include "eeprom_store.h"
#include "snooze.h"

volatile TimeRecord_t g_currentTime;
volatile TimeRecord_t g_alarmTime;
volatile SystemMode_t g_systemMode;
volatile AdjustField_t g_adjustField;
volatile bool g_alarmArmed;
volatile bool g_secondsFlashOn;
volatile bool g_displayDirty;

void FSM_Init(void) {
    g_currentTime.hours = 0;
    g_currentTime.minutes = 0;
    g_currentTime.seconds = 0;
    g_currentTime.is24Hour = true;
    g_currentTime.isPM = false;

    g_alarmTime.hours = 0;
    g_alarmTime.minutes = 0;
    g_alarmTime.seconds = 0;
    g_alarmTime.is24Hour = true;
    g_alarmTime.isPM = false;

    g_systemMode = MODE_NORMAL;
    g_adjustField = ADJUST_NONE;
    g_alarmArmed = false;
    g_secondsFlashOn = true;
    g_displayDirty = true;

    Snooze_Init();
}

void FSM_HandleKey(uint8_t key) {
    if (g_systemMode == MODE_ALARM_SOUND) {
        if (key == '#') {
            g_systemMode = MODE_NORMAL;
            Snooze_Reset();
            Buzzer_Off();
            RGB_SetColor(g_alarmArmed ? LED_YELLOW : LED_OFF);
        } else {
            g_systemMode = MODE_NORMAL;
            Buzzer_Off();
            RGB_SetColor(LED_BLUE);
            Snooze_Trigger(g_currentTime.hours, g_currentTime.minutes);
        }
        g_displayDirty = true;
        return;
    }

    switch (key) {
        case '0':
            g_currentTime.is24Hour = !g_currentTime.is24Hour;
            EEPROM_SaveSettings();
            break;
        case '1':
            g_systemMode = (g_systemMode == MODE_SET_TIME) ? MODE_NORMAL : MODE_SET_TIME;
            g_adjustField = ADJUST_NONE;
            break;
        case '3':
            g_systemMode = (g_systemMode == MODE_SET_ALARM) ? MODE_NORMAL : MODE_SET_ALARM;
            g_adjustField = ADJUST_NONE;
            if (g_systemMode == MODE_SET_ALARM) {
                g_alarmArmed = true;
                Snooze_Reset();
                RGB_SetColor(LED_YELLOW);
            } else {
                EEPROM_SaveSettings();
            }
            break;
        case '4':
            if (g_systemMode == MODE_SET_TIME || g_systemMode == MODE_SET_ALARM) {
                g_adjustField = ADJUST_HOUR;
            }
            break;
        case '5':
            if (g_systemMode == MODE_SET_TIME || g_systemMode == MODE_SET_ALARM) {
                g_adjustField = ADJUST_MINUTE;
            }
            break;
        case '6':
            if (g_systemMode == MODE_SET_TIME) {
                g_currentTime.seconds = (g_currentTime.seconds == 0) ? 59 : 0;
            }
            break;
        case '*':
            g_adjustField = ADJUST_NONE;
            break;
        case '#':
            g_alarmArmed = false;
            Snooze_Reset();
            RGB_SetColor(LED_OFF);
            EEPROM_SaveSettings();
            break;
        default:
            break;
    }

    g_displayDirty = true;
}

void FSM_Update(void) {
    volatile TimeRecord_t *target;

    if (!g_potReady) {
        return;
    }
    g_potReady = false;

    if (g_systemMode == MODE_SET_TIME) {
        target = &g_currentTime;
    } else if (g_systemMode == MODE_SET_ALARM) {
        target = &g_alarmTime;
    } else {
        return;
    }

    if (g_adjustField == ADJUST_HOUR) {
        target->hours = (uint8_t)((g_potValue * 24) / 4096);
        g_displayDirty = true;
    } else if (g_adjustField == ADJUST_MINUTE) {
        target->minutes = (uint8_t)((g_potValue * 60) / 4096);
        g_displayDirty = true;
    }
}
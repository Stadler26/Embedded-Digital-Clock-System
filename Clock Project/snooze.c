#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "snooze.h"

#define SNOOZE_MINUTES 1

volatile bool g_snoozeActive;
static volatile uint8_t s_snoozeHours;
static volatile uint8_t s_snoozeMinutes;

void Snooze_Init(void) {
    g_snoozeActive = false;
    s_snoozeHours = 0;
    s_snoozeMinutes = 0;
}

void Snooze_Trigger(uint8_t currentHours, uint8_t currentMinutes) {
    uint8_t h = currentHours;
    uint8_t m = currentMinutes + SNOOZE_MINUTES;

    if (m >= 60) {
        m -= 60;
        h++;
        if (h >= 24) {
            h = 0;
        }
    }

    s_snoozeHours = h;
    s_snoozeMinutes = m;
    g_snoozeActive = true;
}

bool Snooze_CheckDue(uint8_t currentHours, uint8_t currentMinutes, uint8_t currentSeconds) {
    if (!g_snoozeActive) {
        return false;
    }
    if (currentHours == s_snoozeHours &&
        currentMinutes == s_snoozeMinutes &&
        currentSeconds == 0) {
        g_snoozeActive = false;
        return true;
    }
    return false;
}

void Snooze_Reset(void) {
    g_snoozeActive = false;
}
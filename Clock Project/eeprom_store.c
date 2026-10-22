#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C129.h>
#include "eeprom_store.h"
#include "app_fsm.h"

#define SETTINGS_MAGIC 0x434C4B31

static void Delay(volatile uint32_t count) {
    while (count--) {
    }
}

void EEPROM_Init(void) {
    SYSCTL->RCGCEEPROM |= (1 << 0);
    Delay(50);
    while ((SYSCTL->PREEPROM & (1 << 0)) == 0) {
    }
    while ((EEPROM->EEDONE & (1 << 0)) != 0) {
    }
}

void EEPROM_SaveSettings(void) {
    uint32_t packed;

    packed = (uint32_t)g_alarmTime.hours;
    packed |= ((uint32_t)g_alarmTime.minutes) << 8;
    packed |= (g_currentTime.is24Hour ? 1u : 0u) << 16;
    packed |= (g_alarmArmed ? 1u : 0u) << 17;

    EEPROM->EEBLOCK = 0;
    EEPROM->EEOFFSET = 0;
    EEPROM->EERDWR = SETTINGS_MAGIC;
    while ((EEPROM->EEDONE & (1 << 0)) != 0) {
    }

    EEPROM->EEBLOCK = 0;
    EEPROM->EEOFFSET = 1;
    EEPROM->EERDWR = packed;
    while ((EEPROM->EEDONE & (1 << 0)) != 0) {
    }
}

bool EEPROM_LoadSettings(void) {
    uint32_t magic;
    uint32_t packed;

    EEPROM->EEBLOCK = 0;
    EEPROM->EEOFFSET = 0;
    magic = EEPROM->EERDWR;

    if (magic != SETTINGS_MAGIC) {
        return false;
    }

    EEPROM->EEBLOCK = 0;
    EEPROM->EEOFFSET = 1;
    packed = EEPROM->EERDWR;

    g_alarmTime.hours = (uint8_t)(packed & 0xFF);
    g_alarmTime.minutes = (uint8_t)((packed >> 8) & 0xFF);
    g_currentTime.is24Hour = ((packed >> 16) & 0x1) != 0;
    g_alarmArmed = ((packed >> 17) & 0x1) != 0;

    return true;
}
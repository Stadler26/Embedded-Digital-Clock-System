#ifndef EEPROM_STORE_H
#define EEPROM_STORE_H

#include <stdbool.h>

void EEPROM_Init(void);
void EEPROM_SaveSettings(void);
bool EEPROM_LoadSettings(void);

#endif
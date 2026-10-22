#ifndef SNOOZE_H
#define SNOOZE_H

#include <stdint.h>
#include <stdbool.h>

extern volatile bool g_snoozeActive;

void Snooze_Init(void);
void Snooze_Trigger(uint8_t currentHours, uint8_t currentMinutes);
bool Snooze_CheckDue(uint8_t currentHours, uint8_t currentMinutes, uint8_t currentSeconds);
void Snooze_Reset(void);

#endif
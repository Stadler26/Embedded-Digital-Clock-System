#ifndef BUZZER_H
#define BUZZER_H

#include <stdbool.h>

void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_AlarmTick(bool toneOn);

#endif
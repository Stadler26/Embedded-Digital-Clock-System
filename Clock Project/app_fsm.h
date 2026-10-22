#ifndef APP_FSM_H
#define APP_FSM_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MODE_NORMAL,
    MODE_SET_TIME,
    MODE_SET_ALARM,
    MODE_ALARM_SOUND
} SystemMode_t;

typedef enum {
    ADJUST_NONE,
    ADJUST_HOUR,
    ADJUST_MINUTE
} AdjustField_t;

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    bool is24Hour;
    bool isPM;
} TimeRecord_t;

extern volatile TimeRecord_t g_currentTime;
extern volatile TimeRecord_t g_alarmTime;
extern volatile SystemMode_t g_systemMode;
extern volatile AdjustField_t g_adjustField;
extern volatile bool g_alarmArmed;
extern volatile bool g_secondsFlashOn;
extern volatile bool g_displayDirty;

void FSM_Init(void);
void FSM_HandleKey(uint8_t key);
void FSM_Update(void);

#endif
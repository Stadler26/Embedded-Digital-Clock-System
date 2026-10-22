#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <TM4C129.h>
#include "app_fsm.h"
#include "gpio_keypad.h"
#include "timer_clock.h"
#include "adc_pot.h"
#include "buzzer.h"
#include "rgb_led.h"
#include "uart_display.h"
#include "eeprom_store.h"
#include "idle.h"

int main(void) {
    FSM_Init();

    __disable_irq();

    UART_Init();
    Keypad_Init();
    Timer_Init();
    ADC_Init();
    Buzzer_Init();
    RGB_Init();
    EEPROM_Init();

    __enable_irq();

    if (EEPROM_LoadSettings()) {
        if (g_alarmArmed) {
            RGB_SetColor(LED_YELLOW);
        }
        g_displayDirty = true;
    }

    while (1) {
        if (g_keyEventPending) {
            Keypad_Service();
        }

        if (g_systemMode == MODE_SET_TIME || g_systemMode == MODE_SET_ALARM) {
            ADC_StartConversion();
        }

        FSM_Update();

        if (g_displayDirty) {
            Display_Render();
            g_displayDirty = false;
        }

        Idle_Sleep();
    }

    return 0;
}
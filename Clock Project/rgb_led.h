#ifndef RGB_LED_H
#define RGB_LED_H

typedef enum {
    LED_OFF,
    LED_YELLOW,
    LED_RED,
    LED_BLUE
} LedColor_t;

void RGB_Init(void);
void RGB_SetColor(LedColor_t color);

#endif
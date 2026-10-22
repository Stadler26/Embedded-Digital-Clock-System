#ifndef UART_DISPLAY_H
#define UART_DISPLAY_H

void UART_Init(void);
void UART_WriteChar(char c);
void UART_WriteString(const char *str);
void UART0_Handler(void);
void Display_Render(void);

#endif
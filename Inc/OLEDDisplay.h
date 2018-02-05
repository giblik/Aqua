#ifndef _OLED_H_
#define _OLED_H_

#define BIN	2
#define DEC	10
#define HEX	16

void OLED_Init(void);


void OLED_ClearScreen(void);
void OLED_Refresh(void);
void OLED_ClearScreen(void);
void OLED_draw8x8(uint8_t * buffer8, uint8_t invert);
void OLED_SetCursor(uint8_t x, uint8_t y);
void OLED_SetCursorInBuffer(uint8_t x, uint8_t y);
void OLED_SetCursorNewLine(void);
void OLED_PrintChar(uint8_t ch);
void OLED_PrintString(char * str);
void OLED_PrintfString(char * str);
void OLED_PrintStringInvert(char * str);
void OLED_PrintNumber(int value, uint8_t radix);
void OLED_DrawImageHorizontal128x64(uint8_t * image);
void OLED_DrawPixel(uint8_t x, uint8_t y);
void OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void OLED_DrawHorizontalLine(uint8_t x, uint8_t y, uint8_t width);
void OLED_DrawVerticalLine(uint8_t x, uint8_t y, uint8_t hight);

#endif /* _OLED_H_ */

#include "inttypes.h"
#include "OLEDDisplay.h"
#include "i2c.h"
#include "stdlib.h"
#include "text_font.h"

#define TRUE	1
#define FALSE	0

#define	DATS		0b01000000		// ВСЕ СЛЕДУЮЩИЕ БАЙТЫ будут данными
#define	DAT			0b11000000		// СЛЕДУЮЩИЙ БАЙТ будет данными
#define	COM			0b10000000		// СЛЕДУЮЩИЙ БАЙТ будет командой
//#define		COM			0b00000000
/*						  	  ||D:1/C:0
 * 						  	  |Co
 */

#define SSD1306_ADR 				0x3C		//	OLED display

#define SSD1306_LCDWIDTH            128
#define SSD1306_LCDHEIGHT           64

#define SSD1306_SETCONTRAST 		0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 		0xA5
#define SSD1306_NORMALDISPLAY 		0xA6
#define SSD1306_INVERTDISPLAY 		0xA7
#define SSD1306_DISPLAYOFF 			0xAE
#define SSD1306_DISPLAYON 			0xAF
#define SSD1306_SETDISPLAYOFFSET 	0xD3
#define SSD1306_SETCOMPINS 			0xDA
#define SSD1306_SETVCOMDETECT 		0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 	0xD5
#define SSD1306_SETPRECHARGE 		0xD9
#define SSD1306_SETMULTIPLEX 		0xA8
#define SSD1306_SETLOWCOLUMN 		0x00
#define SSD1306_SETHIGHCOLUMN		0x10
#define SSD1306_SETSTARTLINE 		0x40
#define SSD1306_MEMORYMODE 			0x20
#define SSD1306_COLUMNADDR 			0x21
#define SSD1306_PAGEADDR   			0x22
#define SSD1306_COMSCANINC 			0xC0
#define SSD1306_COMSCANDEC 			0xC8
#define SSD1306_SEGREMAP 			0xA0
#define SSD1306_CHARGEPUMP 			0x8D
#define SSD1306_EXTERNALVCC 		0x1
#define SSD1306_SWITCHCAPVCC 		0x2
// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 	0x2F
#define SSD1306_DEACTIVATE_SCROLL 	0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 	0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 	0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 		0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL	0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 	0x2A

uint8_t posX=0, posY=0;
static uint8_t screenBuff[8][128];

//static void OLED_SendByte(uint8_t byte);
static void OLED_SendCom(uint8_t cmd);
static void OLED_SendByteArr(uint8_t * arr, int count);

//A0 - начало в правом углу; A1 - начало в левом углу
//C0 - начало снизу экрана; C8 - начало сверху экрана
//12 - пр¤ма¤ лини¤, 02, 22 - лини¤ через строчку
//7F - ¤ркость (максимум 0xFF)
const uint8_t init[18]=
{
	0xA8,0x3F,0xD3,0x00,0x40,0xA1,0xC8,0xDA,0x12,
	0x81,0xFF,0xA4,0xA6,0xD5,0x80,0x8D,0x14,0xAF,
};
void OLED_Init(void){
	for(uint8_t i=0;i<18;i++){
		OLED_SendCom(init[i]);
	}
	OLED_ClearScreen();
}
/*static void OLED_SendByte(uint8_t byte){
	i2c_Start(I2C_DISPLAY, SSD1306, I2C_MODE_WRITE);
	i2c_SendByte(I2C_DISPLAY, DAT);
	i2c_SendByte(I2C_DISPLAY, byte);
	i2c_Stop(I2C_DISPLAY);
}*/
static void OLED_SendCom(uint8_t cmd){
	HAL_I2C_Mem_Write(&hi2c2, (SSD1306_ADR<<1), COM, 1, &cmd, 1, 10);	// в іншій версії СОМ == 0х00
}
static void OLED_SendByteArr(uint8_t * arr, int count){
	HAL_I2C_Mem_Write(&hi2c2, (SSD1306_ADR<<1), DATS, 1, arr, count, 100);
}
void OLED_Refresh(void){
	for(uint8_t y=0; y<8; y++){
		OLED_SetCursor(0, y);
		OLED_SendByteArr(screenBuff[y], 128);
	}

}
void OLED_ClearScreen(void){
	for(uint8_t y=0; y<8; y++){
		for(uint8_t x=0; x<128; x++){
			screenBuff[y][x] = 0x00;
		}
	}
	OLED_Refresh();
}
void OLED_draw8x8(uint8_t * buffer8, uint8_t invert){
	uint8_t sumbol;
	for(uint8_t i=0;i<8;i++){
		sumbol = buffer8[i];
		if(invert == TRUE) sumbol = ~sumbol;
		screenBuff[posY][posX+i] = sumbol;
	}
	if(posX+8 >= 127){
		posX = 0;
		if(posY >= 7) posY=0;
		else posY++;
	} else {
		posX += 8;
	}
}
// Page adressing. x=page(0:7), y=col(0:127)
void OLED_SetCursor(uint8_t x, uint8_t y){
	//posX = x;
	//posY = y;
	OLED_SendCom(0xB0 + y);		// page adress
	OLED_SendCom(x & 0x0F);
	OLED_SendCom(0x10 | (x>>4));
}
void OLED_SetCursorInBuffer(uint8_t x, uint8_t y){
	posX = x; posY = y;
}
void OLED_SetCursorNewLine(void){
	posX=0;
	if(posY >= 7) posY=0;
	else posY++;
}
void OLED_PrintChar(uint8_t ch){
	if(ch < 32 || ch > 127) ch=32;

	OLED_draw8x8((uint8_t *)text_font[ch-32], FALSE);
}
// Друк рядка з переносом
void OLED_PrintString(char * str){
	while(*str != 0){
		OLED_PrintChar(*str++);
	}
	//OLED_Refresh();
}
void OLED_PrintStringInvert(char * str){
	while(*str != 0){
		if(*str < 32 || *str > 127) *str=32;
		OLED_draw8x8((uint8_t *)text_font[*str-32], TRUE);
		*str++;
	}
	//OLED_Refresh();
}
void OLED_PrintfString(char * str){
	OLED_SetCursorNewLine();
	OLED_PrintString(str);
}
void OLED_PrintNumber(int value, uint8_t radix){
	char buff[16];
	itoa(value, buff, radix);
	OLED_PrintString(buff);
}
/* Міняю постідовність бітів бо в convert image
 * немає такої функції курва!
 */
uint8_t Invert(uint8_t x)
{
    int base = 256;

    uint8_t  res = 0;
    while (x != 0)
    {
        res += (x & 1) * (base >>= 1);
        x >>= 1;
    }

    return res;
}
void OLED_DrawImageHorizontal128x64(uint8_t * image){
	/*OLED_SendCom(0x20); OLED_SendCom(0x00); // horizontal
	OLED_SendCom(0x21);OLED_SendCom(0x00);OLED_SendCom(127);	// start stop collum adress
	OLED_SendCom(0x22); OLED_SendCom(0x00); OLED_SendCom(7);	// start stop page adress
	OLED_SetCursor(0,0);
	for(uint8_t i=0; i<1024; i++){
		OLED_SendByte(~Invert(image[i]));
	}
	 */
	uint8_t x=0;
	uint8_t y=0;
	for(int i=0; i<1024; i++){
		if(x >127) {x=0; y++;}
			screenBuff[y][x] = ~Invert(image[i]);
		x++;
	}
	OLED_Refresh();
	//OLED_SendByte(~Invert(image[i]));
	//OLED_SendCom(0x20); OLED_SendCom(0x02); // page adress
}
void OLED_DrawPixel(uint8_t x, uint8_t y){
	screenBuff[y/8][x] |= (1 << (y % 8));
}
void OLED_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height){
	for(uint8_t i=0; i<width; i++){		// --------
		OLED_DrawPixel(x+i, y);			// *      *
	}									// *      *
										// ********

	for(uint8_t i=0; i<width; i++){		// ********
		OLED_DrawPixel(x+i, y+height-1);// *      *
	}									// *      *
										// --------

	for(uint8_t i=0; i<height; i++){	// |*******
		OLED_DrawPixel(x, y+i);			// |      *
	}									// |      *
										// |*******

	for(uint8_t i=0; i<height; i++){	// *******|
		OLED_DrawPixel(x+width-1, y+i);	// *      |
	}									// *      |
										// *******|
}
void OLED_DrawHorizontalLine(uint8_t x, uint8_t y, uint8_t width){
	for(uint8_t i=0;i<width;i++){
		OLED_DrawPixel(x+i, y);
	}
}
void OLED_DrawVerticalLine(uint8_t x, uint8_t y, uint8_t hight){
	for(uint8_t i=0;i<hight;i++){
		OLED_DrawPixel(x, y+i);
	}
}

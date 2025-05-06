#include <stdio.h>
#include "stdlib.h"
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "OLED.h"
#include "OLED_Font.h"

void OLED_WriteCmd(uint8_t cmd)
{
    uint8_t Data[] = {    0x00, cmd   };
    HAL_I2C_Master_Transmit(&hi2c2, OLED_ADDRESSS, Data, 2, 10000);
}
void OLED_WriteData(uint8_t dat)
{
    uint8_t Data[] = {    0x40, dat   };
    HAL_I2C_Master_Transmit(&hi2c2, OLED_ADDRESSS, Data, 2, 10000);
}

/*  0 <= y <= 7
    0 <= x <= 127
*/
void OLED_SetCursor(uint8_t y, uint8_t x)
{
    OLED_WriteCmd(0xB0 | y);
    OLED_WriteCmd(0x10 | (x & 0xf0) >> 4);
    OLED_WriteCmd(0x00 | (x & 0x0f));
}

void OLED_Clear(void)
{
    for (uint8_t j = 0; j < 8; ++j)
    {
        OLED_SetCursor(j, 0);
        for (uint8_t i = 0; i < 128; ++i)
            OLED_WriteData(0x00);
    }
}

void OLED_Init(void)
{
    HAL_Delay(1000);

    OLED_WriteCmd(0xAE); //display off
    OLED_WriteCmd(0x20); //Set Memory Addressing Mode    
    OLED_WriteCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    OLED_WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCmd(0xc8); //Set COM Output Scan Direction
    OLED_WriteCmd(0x00); //---set low column address
    OLED_WriteCmd(0x10); //---set high column address
    OLED_WriteCmd(0x40); //--set start line address
    OLED_WriteCmd(0x81); //--set contrast control register
    OLED_WriteCmd(0xff); //亮度调节 0x00~0xff
    OLED_WriteCmd(0xa1); //--set segment re-map 0 to 127
    OLED_WriteCmd(0xa6); //--set normal display
    OLED_WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    OLED_WriteCmd(0x3F); //
    OLED_WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    OLED_WriteCmd(0xd3); //-set display offset
    OLED_WriteCmd(0x00); //-not offset
    OLED_WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    OLED_WriteCmd(0xf0); //--set divide ratio
    OLED_WriteCmd(0xd9); //--set pre-charge period
    OLED_WriteCmd(0x22); //
    OLED_WriteCmd(0xda); //--set com pins hardware configuration
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xdb); //--set vcomh
    OLED_WriteCmd(0x20); //0x20,0.77xVcc
    OLED_WriteCmd(0x8d); //--set DC-DC enable
    OLED_WriteCmd(0x14); //
    OLED_WriteCmd(0xaf); //--turn on oled panel
    
    OLED_Clear();
}

/**
 * @param y 行位置，1 <= y <= 4
 * @param x 列位置，1 <= x <= 16
**/
void OLED_ShowChar(uint8_t y, uint8_t x, char ch)
{
    OLED_SetCursor((y - 1) * 2, (x - 1) * 8);
    for (uint8_t i = 0; i < 8; ++i)
        OLED_WriteData(F8x16t[ch - ' '][i]);
    OLED_SetCursor((y - 1) * 2 + 1, (x - 1) * 8);
    for (uint8_t i = 8; i < 16; ++i)
        OLED_WriteData(F8x16t[ch - ' '][i]);
}

/**
 * @brief 显示字符串 注意：超出范围部分会在同一行覆盖输出
 * @param y 行位置，1 <= y <= 4
 * @param x 列位置，1 <= x <= 16
**/
void OLED_ShowString(uint8_t y, uint8_t x, char str[])
{
    for (uint8_t i = 0; str[i] != '\0'; ++i)
        OLED_ShowChar(y, x + i, str[i]);
}

void OLED_ShowFloat(uint8_t y, uint8_t x, float f)
{
    char str[20];
    sprintf(str, "%.2f", f);
    OLED_ShowString(y, x, str);
}

uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void OLED_ShowSignNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    char Sign = '+';
    if (Number < 0)
    {
        Sign = '-';
        Number = abs(Number);
    }
    OLED_ShowChar(Line, Column, Sign);
    OLED_ShowNum(Line, Column + 1, Number, Length - 1);
}

#ifndef __OLED_H__
#define __OLED_H__

#define OLED_ADDRESSS  0x78

void OLED_SetCursor(uint8_t y, uint8_t x);
void OLED_Clear(void);
void OLED_Init(void);
void OLED_ShowChar(uint8_t y, uint8_t x, char ch);
void OLED_ShowString(uint8_t y, uint8_t x, char str[]);
void OLED_ShowFloat(uint8_t y, uint8_t x, float f);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);

#endif

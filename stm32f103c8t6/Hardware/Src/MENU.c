#include "MENU.h"
#include "OLED.h"
#include "adc.h"

uint8_t MENU_Cursor = 1;
uint8_t MENU_Page = 1;

void MENU_Init(void)
{

}


/*
k1松开/开启电机


k2设置模式/确定：
k3下一条
k4下一页

第一页内容（基本显示）：显示电压，显示电机开关，显示现在的状态，
第二页菜单内容（设置模式）：录入点，回到原点，沿着边线，跟着a4纸
第三页内容（录入点）：右上，左上，左下，右下


开关激光
*/
void MENU_PageInit(void)
{
    if (MENU_Page == 1)
    {
        OLED_ShowString(1,1,"Motor:");
        OLED_ShowString(2,1,"Volt :");
        OLED_ShowString(3,1,"Mode :");
    }
    if (MENU_Page == 2)
    {
        OLED_ShowString(1,1,"LogIn");
        OLED_ShowString(2,1,"Origin");
        OLED_ShowString(3,1,"Frame");
        OLED_ShowString(4,1,"A4");
    }
    if (MENU_Page == 3)
    {
        OLED_ShowString(1,1,"RU1:");
        OLED_ShowString(2,1,"LU2:");
        OLED_ShowString(3,1,"LD3:");
        OLED_ShowString(4,1,"RD4:");
    }
}

void MENU_PageShow(void)
{
    if (MENU_Page == 1)
    {
        OLED_ShowNum(1,7,1,1);//电机状态，1or0
        OLED_ShowFloat(2,7,((float)HAL_ADC_GetValue(&hadc1) / 4096 *3.3));//电压

    }
    if (MENU_Page == 2)
    {
    }
    if (MENU_Page == 3)
    {
    }
}

uint8_t MENU_CursorMove(uint8_t Cursor_Set)
{
    MENU_Cursor = (MENU_Cursor == 4) ? (MENU_Cursor + 1) : 1;
    MENU_Cursor = (MENU_Cursor == 0) ? MENU_Cursor : Cursor_Set;
    OLED_ShowChar(MENU_Cursor, 16, '<');
    return MENU_Cursor;
}

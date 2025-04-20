#include "MENU.h"
#include "OLED.h"
#include "KEY.h"
#include "MOTOR.h"
#include "adc.h"

static uint8_t MENU_Cursor = 1;
static Menu_Page MENU_CurrentPage = Page_Main;

extern MOTOR_State MOTOR_State_Now;
extern float VoltCH2;
extern uint16_t xy_Corner_Set[4][2];

void MENU_Init(void)
{
    MENU_PageInit(Page_Main);
}


/*
k5松开/开启电机


k1设置模式/确定：
k2下一条
k3下一页
k4返回

第一页内容（基本显示）：显示电压，显示电机开关，显示现在的状态，
第二页菜单内容（设置模式）：录入点，回到原点，沿着边线，跟着a4纸
第三页内容（录入点）：右上，左上，左下，右下

*/
void MENU_PageInit(Menu_Page Page)
{
    OLED_Clear();
    MENU_CurrentPage = Page;

    switch (Page)
    {
        case Page_Main:
            OLED_ShowString(1,1,"Motor:");
            OLED_ShowString(2,1,"Volt :");
            OLED_ShowString(3,1,"Mode :");
            MENU_Cursor = 0;
            break;
        case Page_setting:
            OLED_ShowString(1,1,"Corner");
            OLED_ShowString(2,1,"Origin");
            OLED_ShowString(3,1,"Frame");
            OLED_ShowString(4,1,"A4");
            MENU_CursorMove(1);
            break;
        case Page_Points:
            OLED_ShowString(1,1,"RU1:    ,    ");
            OLED_ShowString(2,1,"LU2:    ,    ");
            OLED_ShowString(3,1,"LD3:    ,    ");
            OLED_ShowString(4,1,"RD4:    ,    ");
            MENU_CursorMove(1);
            break;
    }
}

void MENU_PageShow(void)
{
    switch (MENU_CurrentPage)
    {
        case Page_Main:
            OLED_ShowString(1,7,(MOTOR_State_Now == Motor_On) ? "On" : "OFF");
            OLED_ShowFloat(2,7,VoltCH2);
            break;
        case Page_setting:
            /* code */
            break;
        case Page_Points:
            for(uint8_t i = 0; i < 4; i ++)
            {
                if(xy_Corner_Set[i][0])
                {
                    OLED_ShowNum(i + 1, 5, xy_Corner_Set[i][0], 4);
                }
                else
                {
                    OLED_ShowString(i + 1, 6, "x");
                    OLED_ShowNum(i + 1, 7, i + 1, 1);
                }
                if(xy_Corner_Set[i][1])
                {
                    OLED_ShowNum(i + 1, 10, xy_Corner_Set[i][1], 4);
                }
                else
                {
                    OLED_ShowString(i + 1, 11, "y");
                    OLED_ShowNum(i + 1, 12, i + 1, 1);
                }
            }
            break;
    }
}

uint8_t MENU_CursorMove(uint8_t Cursor_Set)
{
    OLED_ShowChar(MENU_Cursor, 16, ' ');
    MENU_Cursor = (MENU_Cursor == 4) ? 1 : (MENU_Cursor + 1);
    MENU_Cursor = (Cursor_Set == 0) ? MENU_Cursor : Cursor_Set;
    OLED_ShowChar(MENU_Cursor, 16, '<');
    return MENU_Cursor;
}

/*
 * @brief k1设置模式/确定
*/
void Key_Act1(void)//
{
    switch (MENU_CurrentPage)
    {
        case Page_Main:
            MENU_PageInit(Page_setting);
            break;
        case Page_setting:
            switch (MENU_Cursor)
            {
            case 1:
                MENU_PageInit(Page_Points);
                break;
            case 2:
                /* 回到原点 */
                break;
            case 3:
                /* 四边框 */
                break;
            case 4:
                /* a4 */
                break;
            }
            break;
        case Page_Points:
            switch (MENU_Cursor)
            {
            case 1:
                /* code */
                break;
            case 2:
                /* code */
                break;
            case 3:
                /* code */
                break;
            case 4:
                /* code */
                break;
            }
            break;
    }
}
/*
 * @brief k2下一条
*/
void Key_Act2(void)
{
    if (MENU_CurrentPage != Page_Main)
    {
        MENU_CursorMove(0);
    }
}
/*
 * @brief k3下一页
*/
void Key_Act3(void)
{

}
/*
 * @brief k4返回
*/
void Key_Act4(void)
{
    switch (MENU_CurrentPage)
    {
    case Page_Main:
        /* 啥也不干 */
        break;
    
    case Page_setting:
        MENU_PageInit(Page_Main);
        break;
    
    case Page_Points:
        MENU_PageInit(Page_setting);
        break;
    }
}

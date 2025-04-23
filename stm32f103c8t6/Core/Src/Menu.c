#include "Menu.h"
#include "OLED.h"
#include "KEY.h"
#include "MOTOR.h"
#include "Coordinate.h"
#include "Control.h"

static Menu_State menu = {
    .Cursor = 1,
    .Page = Page_Main,
};

extern float VoltCH2;


void Menu_Init(void)
{
    Menu_PageInit(Page_Main);
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
void Menu_PageInit(Menu_Page Page)
{
    OLED_Clear();
    menu.Page = Page;

    switch (Page)
    {
        case Page_Main:
            OLED_ShowString(1,1,"Motor:");
            OLED_ShowString(2,1,"Volt :");
            OLED_ShowString(3,1,"Mode :");
            menu.Cursor = 0;
            break;
        case Page_setting:
            OLED_ShowString(1,1,"Corner");
            OLED_ShowString(2,1,"Origin");
            OLED_ShowString(3,1,"Frame");
            OLED_ShowString(4,1,"A4");
            Menu_CursorMove(1);
            break;
        case Page_Calib:
            OLED_ShowString(1,1,"RU1:    ,    ");
            OLED_ShowString(2,1,"LU2:    ,    ");
            OLED_ShowString(3,1,"LD3:    ,    ");
            OLED_ShowString(4,1,"RD4:    ,    ");
            Menu_CursorMove(1);
            break;
    }
}

void Menu_PageShow(void)
{
    switch (menu.Page)
    {
        case Page_Main:
            OLED_ShowString(1,7,(motor_drive_set.Lock == Locked) ? " Lock " : "Unlock");//这里要定义一个变量接收电机总开关的情况
            OLED_ShowFloat(2,7,VoltCH2);
            OLED_ShowString(3,7,(Current_Mode == Mode_Joystick) ? "Joystick" : 
                (Current_Mode == Mode_Origin) ? "Origin" : 
                (Current_Mode == Mode_Square) ? "Square" : 
                (Current_Mode == Mode_A4Paper) ? "A4Paper" : "None");
            break;
        case Page_setting:
            /* code */
            break;
        case Page_Calib:
            for(uint8_t i = 0; i < 4; i ++)
            {
                if(sys_set.Calib_Point[i].x)
                {
                    OLED_ShowNum(i + 1, 5, sys_set.Calib_Point[i].x, 4);
                }
                else
                {
                    OLED_ShowString(i + 1, 6, "x");
                    OLED_ShowNum(i + 1, 7, i + 1, 1);
                }
                if(sys_set.Calib_Point[i].y)
                {
                    OLED_ShowNum(i + 1, 10, sys_set.Calib_Point[i].y, 4);
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

uint8_t Menu_CursorMove(uint8_t Cursor_Set)
{
    OLED_ShowChar(menu.Cursor, 16, ' ');
    menu.Cursor = (menu.Cursor == 4) ? 1 : (menu.Cursor + 1);
    menu.Cursor = (Cursor_Set == 0) ? menu.Cursor : Cursor_Set;
    OLED_ShowChar(menu.Cursor, 16, '<');
    return menu.Cursor;
}

/*
 * @brief k1设置模式/确定
*/
void Key_Act1(void)//
{
    switch (menu.Page)
    {
        case Page_Main:
            Menu_PageInit(Page_setting);
            break;
        case Page_setting:
            switch (menu.Cursor)
            {
            case 1:
                Menu_PageInit(Page_Calib);
                break;
            case 2:
                Control_SetMode(Mode_Origin);
                Menu_PageInit(Page_Main);
                break;
            case 3:
                Control_SetMode(Mode_Square);
                Menu_PageInit(Page_Main);
                break;
            case 4:
                Control_SetMode(Mode_A4Paper);
                Menu_PageInit(Page_Main);
                break;
            }
            break;
        case Page_Calib://将sys_set.Pixel_Corner_Set的值写入当前光标位置的值
            sys_set.Calib_Point[menu.Cursor - 1] = Point_Queue_Dequeue(&(sys_set.Cam_Point));
            for(uint8_t i = 0; i < 4; i ++)
            {
                if (sys_set.Calib_Point[i].x == 65535) break;
                Coordinate_Init();
            }
            break;
    }
}
/*
 * @brief k2下一条
*/
void Key_Act2(void)
{
    if (menu.Page != Page_Main)
    {
        Menu_CursorMove(0);
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
    switch (menu.Page)
    {
    case Page_Main:
        /* 啥也不干 */
        break;
    
    case Page_setting:
        Menu_PageInit(Page_Main);
        break;
    
    case Page_Calib:
        Menu_PageInit(Page_setting);
        break;
    }
}

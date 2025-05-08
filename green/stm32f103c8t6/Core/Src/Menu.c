#include "Menu.h"
#include "OLED.h"
#include "KEY.h"
#include "MOTOR.h"
#include "Coordinate.h"
#include "Control.h"
#include "queue.h"
#include "JOYSTICK.h"

static Menu_State menu = {
    .Cursor = 1,
    .Page = Page_Main1,
};

uint8_t main3tag = 0;//0队名1人名2学校时间

extern float VoltCH2;
extern uint32_t time;

void Menu_Init(void)
{
    Menu_PageInit(Page_Main1);
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
        case Page_Main1:
        // OLED_ShowString(1, 1, "Laser Ctrl     ");
        // OLED_ShowString(1, 12, (laser.Colour[0] == 'R') ? "Red" : "Gre");
        // OLED_ShowString(2, 1, "Volt :");
        // OLED_ShowString(3, 1, "Mode :");
        // OLED_ShowString(4, 1, "Time :");
        menu.Cursor = 0;
        break;
        
        case Page_Main2:
        OLED_ShowString(1, 1, "Set:    ,    ");
        OLED_ShowString(2, 1, "Now:    ,    ");
        OLED_ShowString(3, 1, "Joy:    ,    ");
        OLED_ShowString(4, 1, "screen:        m");
        menu.Cursor = 0;
        break;

        case Page_Main3:
        OLED_ShowString(1, 1, "Motor:");
        OLED_ShowString(2, 1, "Laser:");
        OLED_ShowString(3, 1, "Beep :");
        switch (main3tag)
        {
            case 0:
            OLED_ShowString(4, 1, "---Yibai Team---");
            break;
            case 1:
            OLED_ShowString(4, 1, "--LBJ--CRY--SY--");
            break;
            case 2:
            OLED_ShowString(4, 1, "--SCUT--202504--");
            break;
        }
        Menu_CursorMove(1);
        break;
    }
}

void Menu_PageShow(void)
{
    switch (menu.Page)
    {
        case Page_Main1:

        //以下为调试代码
        OLED_ShowNum(1,1,sys_set.Flag.Arrive,1);
        OLED_ShowString(2,1,(motor_drive_set.Lock == Locked) ? " Lock " : "Unlock");
        OLED_ShowNum(3, 5, laser.Set_Pix.x, 4);
        OLED_ShowNum(3, 10, laser.Set_Pix.y, 4);
        OLED_ShowString(4, 3, (motor_1L.Dir == Left) ? "Left" : (motor_1L.Dir == Right) ? "Righ" : " Mid");
        OLED_ShowString(4, 10, (motor_2H.Dir == Up) ? " Up " : (motor_2H.Dir == Down) ? "Down" : " Mid");
        

        break;

        case Page_Main2:
        //目标点位
        OLED_ShowNum(1, 5, laser.Set_Pix.x, 4);
        OLED_ShowNum(1, 10, laser.Set_Pix.y, 4);
        //当前点位
        OLED_ShowNum(2, 5, laser.Now_Pix.x, 4);
        OLED_ShowNum(2, 10, laser.Now_Pix.y, 4);
        //摇杆情况
        OLED_ShowString(3, 5, (Joy_Get_X() == Left) ? "Left" : (Joy_Get_X() == Right) ? "Righ" : " Mid");
        OLED_ShowString(3, 10, (Joy_Get_Y() == Up) ? " Up " : (Joy_Get_Y() == Down) ? "Down" : " Mid");
        //屏幕距离
        OLED_ShowFloat(4, 8, sys_set.screen_dis);
        break;

        case Page_Main3:
        OLED_ShowString(1, 7, (motor_1L.En == DISABLE) ? "Disable" : (motor_2H.En == DISABLE) ? "Disable" : "Enable");
        OLED_ShowString(2, 7, (HAL_GPIO_ReadPin(Laser_GPIO_Port, Laser_Pin) == 1) ?  "On " : "OFF");
        OLED_ShowString(3, 7, (HAL_GPIO_ReadPin(Beep_GPIO_Port, Beep_Pin) == 1) ?  "On " : "OFF");
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
        case Page_Main1:
            if (motor_drive_set.Lock == Locked)
            {
                motor_drive_set.Lock = Unlocked;
                motor_1L.Lock = Unlocked;
                motor_2H.Lock = Unlocked;
                sys_set.Flag.Arrive = 1;
            }
            else
            {
                motor_drive_set.Lock = Locked;
                motor_1L.Lock = Locked;
                motor_2H.Lock = Locked;
            } 
        break;

        case Page_Main3:
        switch (menu.Cursor)
        {
            case 1:
            if(motor_1L.En == DISABLE && motor_2H.En == DISABLE)
            {
                motor_1L.En = ENABLE;
                motor_2H.En = ENABLE;
            }
            else
            {
                motor_1L.En = DISABLE;
                motor_2H.En = DISABLE;        
            }
            HAL_GPIO_WritePin(motor_1L.Pin_Config.En_Port, motor_1L.Pin_Config.En_Pin, (GPIO_PinState)motor_1L.En);
            HAL_GPIO_WritePin(motor_2H.Pin_Config.En_Port, motor_2H.Pin_Config.En_Pin, (GPIO_PinState)motor_2H.En);
            break;

            case 2:

            HAL_GPIO_TogglePin(Laser_GPIO_Port, Laser_Pin);
            break;

            case 3:
            // Control_SetMode(Mode_Square);
            // Menu_PageInit(Page_Main);
            //此处计时，给一个短促的beep
            break;
                
            case 4:
            main3tag %= 3;
            Menu_PageInit(Page_Main3);
            break;
        }
        break;
        
        case Page_Error:
        Menu_PageInit(Page_Main1);
        break;
    }
}
/*
 * @brief k2下一条
*/
void Key_Act2(void)
{
    if (menu.Page != Page_Main1 && menu.Page != Page_Main2 && menu.Page != Page_Wait && menu.Page != Page_Error)
    {
        Menu_CursorMove(0);
    }
}
/*
 * @brief k3下一页
*/
void Key_Act3(void)
{
    switch (menu.Page)
    {
        case Page_Main1:
        Menu_PageInit(Page_Main2);
        break;

        case Page_Main2:
        Menu_PageInit(Page_Main3);
        break;

        case Page_Main3:
        Menu_PageInit(Page_Main1);
        break;
        
        case Page_Error:
        Menu_PageInit(Page_Main1);
        break;
        
    }
}
/*
 * @brief k4返回
*/
void Key_Act4(void)
{
    switch (menu.Page)
    {
        case Page_Main1:
        if (sys_set.Flag.Init == 0) Menu_PageInit(Page_Disinit);
        break;

        case Page_Main2:
        Menu_PageInit(Page_Main1);
        break;

        case Page_Main3:
        Menu_PageInit(Page_Main2);
        break;

        case Page_Error:
        Menu_PageInit(Page_Main1);
        break;
        
    }
}


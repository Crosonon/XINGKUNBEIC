#ifndef __MENU_H__
#define __MENU_H__

#include "KEY.h"

typedef enum 
{
    Page_Main1 = 1,//设备名字，电池电压，当前模式（初始化否？上锁否？），用时
    Page_Main2,//目标点位置，当前点位置，摇杆情况，超声波距离
    Page_Main3,//电机使能，激光开关，蜂鸣器开关，易柏队

    Page_set1,//回到原点、按顺序四角、顺时针a4、摇杆
    Page_set2,//可能有其他模式，比如追随绿光，走到摄像头刚发来的位置之类的，先不写
    Page_Square,//四点坐标，初始化前会显示x1y1

    Page_Wait,//正在取点并平均
    Page_Disinit,//还没初始化呢哥,第三行跳转init，第四行跳转回首页
    Page_Error,//没有点可以写入之类的
} Menu_Page;

typedef struct
{
    Menu_Page Page;
    uint8_t Cursor;

} Menu_State;


void Menu_Init(void);
void Menu_PageInit(Menu_Page Page);
void Menu_PageShow(void);
uint8_t Menu_CursorMove(uint8_t Cursor_Set);


#endif

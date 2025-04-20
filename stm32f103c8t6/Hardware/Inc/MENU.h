#ifndef __MENU_H__
#define __MENU_H__

#include "KEY.h"

typedef enum 
{
    Page_Main = 1,
    Page_setting,
    Page_Points
} Menu_Page;

void MENU_Init(void);
void MENU_PageInit(Menu_Page Page);
void MENU_PageShow(void);
uint8_t MENU_CursorMove(uint8_t Cursor_Set);


#endif

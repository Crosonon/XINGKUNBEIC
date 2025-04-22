#ifndef __MENU_H__
#define __MENU_H__

#include "KEY.h"

typedef enum 
{
    Page_Main = 1,
    Page_setting,
    Page_Calib,
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

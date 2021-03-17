#include "rtthread.h"
#include "GUI.h"
extern void GUIPROP_X_DispChar(U16P c);
extern int GUIPROP_X_GetCharDistX(U16P c);

GUI_CONST_STORAGE GUI_CHARINFO GUI_FontHZ64_CharInfo[2] = {
    { 32,   32, 4, (void*)"0"},
    { 64,   64, 8, (void*)"0"},
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ64_PropHZ = {
    0xA1A1,
    0xFFFF,
    &GUI_FontHZ64_CharInfo[1],
    (void *)0,
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ64_PropASC = {
    0x0000,
    0x007F,
    &GUI_FontHZ64_CharInfo[0],
    (void GUI_CONST_STORAGE *)&GUI_FontHZ64_PropHZ,
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ64 = {
    GUI_FONTTYPE_PROP_USER,
    64,
    64,
    1,
    1,
    (void GUI_CONST_STORAGE *)&GUI_FontHZ64_PropASC
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ64x2 = {
    GUI_FONTTYPE_PROP_USER,
    64,
    64,
    2,
    2,
    (void GUI_CONST_STORAGE *)&GUI_FontHZ64_PropASC
};


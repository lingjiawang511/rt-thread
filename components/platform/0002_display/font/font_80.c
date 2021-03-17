#include "rtthread.h"
#include "GUI.h"
extern void GUIPROP_X_DispChar(U16P c);
extern int GUIPROP_X_GetCharDistX(U16P c);

GUI_CONST_STORAGE GUI_CHARINFO GUI_FontHZ80_CharInfo[2] = {
    { 40,   40, 5, (void*)"0"},
    { 80,   80, 10, (void*)"0"},
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ80_PropHZ = {
    0xA1A1,
    0xFFFF,
    &GUI_FontHZ80_CharInfo[1],
    (void *)0,
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ80_PropASC = {
    0x0000,
    0x007F,
    &GUI_FontHZ80_CharInfo[0],
    (void GUI_CONST_STORAGE *)&GUI_FontHZ80_PropHZ,
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ80 = {
    GUI_FONTTYPE_PROP_USER,
    64,
    64,
    1,
    1,
    (void GUI_CONST_STORAGE *)&GUI_FontHZ80_PropASC
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ80x2 = {
    GUI_FONTTYPE_PROP_USER,
    64,
    64,
    2,
    2,
    (void GUI_CONST_STORAGE *)&GUI_FontHZ80_PropASC
};


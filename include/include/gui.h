#ifndef __GUI_H__
#define __GUI_H__

#include "stm32f4xx_hal.h"
#include "drv_lcd.h"
typedef unsigned char  u8;
typedef unsigned short  u16;
typedef unsigned int  u32;
#define LCD_TFT 1

#define CURVE_Y2  95
#define CURVE_Y1  25
#define CURVE_X2  128
#define CURVE_X1  5
void GUI_LCD_DisplayOn(void);
void GUI_LCD_DisplayOff(void);
void GUI_DrawPoint(u16 x, u16 y, u16 color);
void GUI_LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void Draw_Circle(u16 x0, u16 y0, u16 fc, u8 r);
void LCD_ShowChar(u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 size, u8 mode);
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size);
void LCD_Show2Num(u16 x, u16 y, u16 num, u8 len, u8 size, u8 mode);
void LCD_ShowString(u16 x, u16 y, u8 size, u8 *p, u8 mode);
void LCD_Gram_ShowChar ( u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 size, u8 mode );
void GUI_DrawFont16(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode);
void GUI_DrawFont24(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode);
void GUI_DrawFont32(u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode);
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str, u8 size, u8 mode);
void LCD_Gram_ShowChar ( u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 size, u8 mode );
void Gui_Drawbmp16(u16 x, u16 y, const unsigned char *p); //œ‘ æ40*40 QQÕº∆¨
void gui_circle(int xc, int yc, u16 c, int r, int fill);
void Gui_StrCenter(u16 x, u16 y, u16 fc, u16 bc, u8 *str, u8 size, u8 mode);
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void register_lcd_dev(_lcd_dev *p);
u32 mypow ( u8 m, u8 n );
void Gui_Lcd_Clear(u16 color);
void Gui_Lcd_Clear_Part(u16 sx, u16 sy, u16 color);
u8 dprintf_o(int val, u8 dot, u16 sx, u16 sy, u8 size, u8 mode);
u8 dprintf_9(int val, u8 dot, u16 sx, u16 sy, u8 size, u8 mode);
//void Gui_Lcd_Unit(ModifyDataType mdata, u8 size);
void Gui_Lcd_Battery(u16 sx, u16 sy, u8 batpercent, u16 consumecolor, u16 batcolor);
void Gui_Lcd_Interface(void);
//void Gui_display_error(u16 x, u16 y, Display_Blink_Type blinktype, u8 size, u8 mode);
void Gui_Usb_Icon(void);
void Gui_Refresh_Gram(void);
void Gui_Clear_Gram(void);
//void Gui_Curve(Curve_Typde *curve, u8 gui_channal);
void Display_Sub_Param(u16 sx, u16 sy, u8 size, u8 select_index);
void Display_Main_Param(void );
void Gui_Deinit_Lcd(void);
void Gui_clear_battery(u16 sx, u16 sy, u16 width, u16 lenth);
void Gui_battery_badstate(u16 sx, u16 sy);
void Gui_Control_Contrast(u8 percent);
void set_display_switch_time(u16 s_displaytime);
void Auto_Switch_Display(void);
#endif


#include <rthw.h>
#include <rtthread.h>
#include "stdio.h"
#include "string.h"
#include "include/HeadType.h"
#include "stdlib.h"
#include "errno.h"
#include "stdlib.h"
#include "include/font.h"
#include "include/gui.h"


extern TagParam    param;
extern ModifyDataType ModifyMenuData;
extern unsigned long TimeInterval;
extern unsigned char dispaly_pwr_state;
extern TagWorkState     WorkState;
extern unsigned char dewoutofrangeFlag;
extern unsigned char SensorOnlineFlag;
extern TagLang lang;

//#include "..\kernel\menu\front.H"
static u16 POINT_COLOR = BLUE;
static u16 BACK_COLOR  = GREEN;
static _lcd_dev *pdev;
static u16 display_swtich_time = 0;
u8 usb_displaystate;
extern  __IO unsigned long bDeviceState;
Curve_Typde Curve;
extern trend_state get_temp_data_trend(void);
extern trend_state get_humi_data_trend(void);
extern trend_state get_dewpoint_data_trend(void);
extern trend_state get_ppmv_data_trend(void);
//extern void init_alarm_ch_param(u8 alarm_ch);
extern void deinit_probe_data_trend(void);
void Display_Alarm_Icon(void);

uint8_t sensor_err_count;
uint8_t get_detectdew_result(void)
{
	return 0;
}
int32_t get_dewpoint_scale(void)
 {
	return 0;
 }

void set_detectdew_result(u8 value)
{
	
}
int32_t target_is_watch_meter(void)
{

}

static void Delay(unsigned long time)
{
    unsigned long i, j;
    unsigned long mstime = 24000;
    mstime = 24000;
    for (j = 0; j < time; j++) {
        for (i = 0; i < mstime; i++);
    }
}

void GUI_LCD_DisplayOn(void)
{
    pdev->LCD_DisplayOn ();
//    Gui_Lcd_Interface();
//    Gui_Lcd_Unit(ModifyMenuData, 32);
//    RTCFlag = UPDATE_DATE_AND_TIME;           //RTC日期刷新
}
void GUI_LCD_DisplayOff(void)
{
    pdev->LCD_DisplayOff ();
}
void GUI_DrawPoint ( u16 x, u16 y, u16 color )
{
    pdev->LCD_Fast_DrawPoint ( x, y, color );
}

void GUI_LCD_Fill ( u16 sx, u16 sy, u16 ex, u16 ey, u16 color )
{
    pdev->LCD_Fill (sx, sy, ex, ey, color );
    pdev->LCD_Set_Window ( 0, 0, pdev->width - 1, pdev->height - 1 ); //恢复窗口设置为全屏
}

void LCD_DrawLine ( u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if ( delta_x > 0 ) incx = 1; //设置单步方向
    else if ( delta_x == 0 ) incx = 0; //垂直线
    else {
        incx = -1;
        delta_x = -delta_x;
    }

    if ( delta_y > 0 ) incy = 1;
    else if ( delta_y == 0 ) incy = 0; //水平线
    else {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y ) distance = delta_x; //选取基本增量坐标轴
    else distance = delta_y;

    for ( t = 0; t <= distance + 1; t++ ) { //画线输出
        GUI_DrawPoint (uRow, uCol, color );
        xerr += delta_x ;
        yerr += delta_y ;

        if ( xerr > distance ) {
            xerr -= distance;
            uRow += incx;
        }

        if ( yerr > distance ) {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void LCD_DrawRectangle ( u16 x1, u16 y1, u16 x2, u16 y2, u16 color )
{
    LCD_DrawLine ( x1, y1, x2, y1, color );
    LCD_DrawLine ( x1, y1, x1, y2, color );
    LCD_DrawLine ( x1, y2, x2, y2, color );
    LCD_DrawLine ( x2, y1, x2, y2, color );
}

void LCD_DrawFillRectangle ( u16 x1, u16 y1, u16 x2, u16 y2 )
{
    GUI_LCD_Fill ( x1, y1, x2, y2, POINT_COLOR );
}

void _draw_circle_8 ( int xc, int yc, int x, int y, u16 c )
{
//    GUI_DrawPoint ( xc + x, yc + y, c );
//    GUI_DrawPoint ( xc - x, yc + y, c );
//    GUI_DrawPoint ( xc + x, yc - y, c );
//    GUI_DrawPoint ( xc - x, yc - y, c );
//    GUI_DrawPoint ( xc + y, yc + x, c );
//    GUI_DrawPoint ( xc - y, yc + x, c );
//    GUI_DrawPoint ( xc + y, yc - x, c );
//    GUI_DrawPoint ( xc - y, yc - x, c );

    pdev->LCD_DrawPoint ( xc + x, yc + y, c );
    pdev->LCD_DrawPoint ( xc - x, yc + y, c );
    pdev->LCD_DrawPoint ( xc + x, yc - y, c );
    pdev->LCD_DrawPoint ( xc - x, yc - y, c );
    pdev->LCD_DrawPoint ( xc + y, yc + x, c );
    pdev->LCD_DrawPoint ( xc - y, yc + x, c );
    pdev->LCD_DrawPoint ( xc + y, yc - x, c );
    pdev->LCD_DrawPoint ( xc - y, yc - x, c );
}

void gui_circle ( int xc, int yc, u16 c, int r, int fill )
{
    int x = 0, y = r, yi, d;
    d = 3 - 2 * r;

    if ( fill ) {
        while ( x <= y ) {
            for ( yi = x; yi <= y; yi++ )
                _draw_circle_8 ( xc, yc, x, yi, c );

            if ( d < 0 ) {
                d = d + 4 * x + 6;
            } else {
                d = d + 4 * ( x - y ) + 10;
                y--;
            }

            x++;
        }
    } else {
        while ( x <= y ) {
            _draw_circle_8 ( xc, yc, x, y, c );

            if ( d < 0 ) {
                d = d + 4 * x + 6;
            } else {
                d = d + 4 * ( x - y ) + 10;
                y--;
            }

            x++;
        }
    }
}
#if LCD_TFT==1
void LCD_ShowChar ( u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 size, u8 mode )
{
    u8 temp, t1;
    u16 y0 = y;
    static u16 t, csize;
    if (size == 64) {
        num = num - '0';
    } else {
        num = num - ' '; //得到偏移后的值
    }
    pdev->LCD_Set_Window ( x, y, x + size / 2 - 1, y + size - 1 ); //设置单个文字显示窗口
    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); //μ?μ?×?ì?ò???×?·???ó|μ??ó?ˉ?ù??μ?×??úêy
    if (size != 12 && size != 16 && size != 24 && size != 32 && size != 48 && size != 64 && size != 80)return; //2??§3?μ?size
    for (t = 0; t < csize; t++) {
        if (size == 12)temp = asc_12_06[num][t];    //μ÷ó?1206×?ì?
        else if (size == 16)temp = asc_16_08[num][t]; //μ÷ó?1608×?ì?
        else if (size == 24)temp = asc_24_12[num][t]; //μ÷ó?2412×?ì?
        else if (size == 32)temp = asc_32_16[num][t];
        else return;
        for (t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80) {
                GUI_DrawPoint(x, y, fc);
            } else if ((mode == 0) || (mode == 2)) {
                GUI_DrawPoint(x, y, bc);
            }
            temp <<= 1;
            y++;
            if (y >= pdev->height)return;   //超区域了
            if ((y - y0) == size) {
                y = y0;
                x++;
                if (x >= pdev->width)return; //超区域了
                break;
            }
        }
    }
    pdev->LCD_Set_Window ( 0, 0, pdev->width - 1, pdev->height - 1 ); //恢复窗口为全屏
}
void LCD_Gram_ShowChar ( u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 size, u8 mode )
{
    u8 temp;
    u8 pos, j, t;
    u8 y0 = y;
    u8 y1;
    u8 x0 = x;
    if (size == 8) {
//        LCD_Gram_ShowChar_Size0806( x, y, fc, bc, num, mode );
        return;
    } else if (size == 12) {
////        LCD_Gram_ShowChar_Size1206( x, y, fc, bc, num, mode );
        return;
    } else if (size == 64) {
        num = num - '0';
    } else {
        num = num - ' '; //得到偏移后的值
    }
    for ( pos = 0; pos < (size + 4) / 8; pos++ ) {
        y1 = y0 + ((size + 4) / 8 - pos - 1) * 8;
        y = y1;
        x = x0;
        for ( t = 0; t < size / 2; t++ ) {
            if (size == 64)temp = asc_32_64[num][pos * 32 + t]; //调用6464字体
//            if (size == 40)temp = asc_20_40[num][pos * 20 + t]; //调用6464字体
            else if ( size == 32 ) temp = asc_32_16[num][pos * 16 + t]; //调用3232字体
            else temp = asc_16_08[num][pos * 8 + t]; //调用1608字体
            if ( !mode ) { //非叠加方式
                temp = temp;
            } else {
                temp = ~temp;
            }
            for ( j = 0; j < 8 ; j++ ) {
                if ( temp & 0x80 ) {
                    GUI_DrawPoint( x, y, 0XFF); //画点
                } else {
                    GUI_DrawPoint ( x, y, 0X00); //画点
                }
                temp <<= 1;
                y++;
                if ((y - y1) == 8) {
                    y = y1;
                    x++;
                    break;
                }
            }
        }
    }
}
#else
void LCD_ShowChar ( u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 size, u8 mode )
{
    u8 temp;
    u8 pos, t;
    u8 nrow;
    u8 xtemp;
    if (target_is_watch_meter()) {
        if ((x >= 128) || (y >= 8)) {
            return;
        }
    } else {
        if ((x >= 128) || (y >= 12)) {
            return;
        }
    }
    if (size >= 48) {
        num = num - '0';
    } else {
        num = num - ' '; //得到偏移后的值
    }
    pdev->LCD_Set_Window ( x, y, x + size / 2 - 1, y + size / 8 - 1 ); //设置单个文字显示窗口
    if (size == 8) {
        nrow = 6;
    } else if (((size <= 40) && (num == 14)) || ((size <= 64) && (num == 12))) {
        nrow = size / 2 - (size / 16 * 4);
    } else {
        nrow = size / 2;
    }
    if (target_is_watch_meter()) {
        if ((x > (128 - size / 2)) || (y * 8 > (64 - size))) { //地址超出显示范围
            return;
        }
    } else {
        if ((x > (128 - size / 2)) || (y * 8 > (96 - size))) { //地址超出显示范围
            return;
        }
    }
    if ( !mode ) { //非叠加方式
        for ( pos = 0; pos < size / 8; pos++ ) {
//            pdev->LCD_WR_REG(0Xb0 + pos + y); //lcd12896没有窗口设置，需要手动调整坐标
            if (target_is_watch_meter()) {
                pdev->LCD_WR_REG(0Xb0 + pos + y); //lcd12864没有窗口设置，需要手动调整坐标
                xtemp = x + 4; //反屏使用，需要调整X轴四个点，因为屏的线是132
                pdev->LCD_WR_REG(0x10 + xtemp / 16);
                pdev->LCD_WR_REG(0x00 + xtemp % 16);
            } else {
                pdev->LCD_WR_REG(0Xb4 + pos + y); //lcd12896没有窗口设置，需要手动调整坐标
                pdev->LCD_WR_REG(0x10 + x / 16);
                pdev->LCD_WR_REG(0x00 + x % 16);
            }
            for ( t = 0; t < nrow; t++ ) {
                if (size == 64)temp = asc_32_64[num][pos * 32 + t]; //调用6464字体
                else if (size == 48)temp = asc_24_48[num][pos * 24 + t]; //调用6464字体
                else if (size == 40)temp = asc_20_40[num][pos * 20 + t]; //调用6464字体
                else if ( size == 32 ) temp = asc_16_32[num][pos * 16 + t]; //调用3232字体
                else if ( size == 24 ) temp = asc_12_24[num][pos * 12 + t]; //调用3232字体
                else if ( size == 12 ) temp = asc2_1206[num][pos + t]; //调用1206字体
                else if (size == 8)  temp = asc2_0806[num][pos + t];
                else temp = asc2_1608[num][pos * 8 + t]; //调用1608字体
                pdev->LCD_WR_DATA(temp);
                if (!target_is_watch_meter()) { //12896是四级灰度，需要写两遍
                    pdev->LCD_WR_DATA(temp);
                }
            }
        }
    } else { //叠加方式
        for ( pos = 0; pos < size / 8; pos++ ) {
//            pdev->LCD_WR_REG(0Xb0 + pos + y); //lcd12896不反屏需要设置地址为0Xb0，反屏设置为0Xb4
            if (target_is_watch_meter()) {
                pdev->LCD_WR_REG(0Xb0 + pos + y); //lcd12864
                xtemp = x + 4; //反屏使用，需要调整X轴四个点，因为屏的线是132
                pdev->LCD_WR_REG(0x10 + xtemp / 16);
                pdev->LCD_WR_REG(0x00 + xtemp % 16);
            } else {
                pdev->LCD_WR_REG(0Xb4 + pos + y); //lcd12896没有窗口设置，需要手动调整坐标
                pdev->LCD_WR_REG(0x10 + x / 16);
                pdev->LCD_WR_REG(0x00 + x % 16);
            }
            for ( t = 0; t < nrow; t++ ) {
                if (size == 64)temp = asc_32_64[num][pos * 32 + t]; //调用6464字体
                if (size == 40)temp = asc_20_40[num][pos * 20 + t]; //调用6464字体
                else if ( size == 32 ) temp = asc_16_32[num][pos * 16 + t]; //调用3232字体
                else if ( size == 12 ) temp = asc2_1206[num][pos + t]; //调用1206字体
                else if (size == 8)  temp = asc2_0806[num][pos + t];
                else temp = asc2_1608[num][pos * 8 + t]; //调用1608字体
                pdev->LCD_WR_DATA(~temp);
                if (!target_is_watch_meter()) {
                    pdev->LCD_WR_DATA(~temp);
                }
            }
        }
    }

    pdev->LCD_Set_Window ( 0, 0, pdev->width - 1, pdev->height - 1 ); //恢复窗口为全屏
}
static void LCD_Gram_ShowChar_Size0806( u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 mode )
{
    u8 temp;
    u8 j, t;
    u8 y1 = y;
    u8 x0 = x;

    num = num - ' '; //得到偏移后的值
    y = y1;
    x = x0;
    for ( t = 0; t < 6; t++ ) {
        temp = asc2_0806[num][ t ]; //调用1206字体
        if ( !mode ) { //非叠加方式
            temp = temp;
        } else {
            temp = ~temp;
        }
        for ( j = 0; j < 8 ; j++ ) {
            if ( temp & 0x80 ) {
                pdev->LCD_DrawPoint ( x, y, 0XFF); //画点
            } else {
                pdev->LCD_DrawPoint ( x, y, 0X00); //画点
            }
            temp <<= 1;
            y++;
            if ((y - y1) == 8 ) {
                y = y1;
                x++;
                break;
            }
        }
    }
}
static void LCD_Gram_ShowChar_Size1206( u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 mode )
{
    u8 temp;
    u8 pos, j, t;
    u8 y0 = y;
    u8 y1;
    u8 x0 = x;

    num = num - ' '; //得到偏移后的值
    for ( pos = 0; pos < 2; pos++ ) {
        y1 = y0 + (1 - pos ) * 4;
        y = y1;
        x = x0;
        for ( t = 0; t < 6; t++ ) {
            temp = asc2_1206[num][pos * 6 + t]; //调用1206字体
            if (pos == 1) {
                temp <<= 4;
            }
            if ( !mode ) { //非叠加方式
                temp = temp;
            } else {
                temp = ~temp;
            }
            for ( j = 0; j < 8 ; j++ ) {
                if ( temp & 0x80 ) {
                    pdev->LCD_DrawPoint ( x, y, 0XFF); //画点
                } else {
                    pdev->LCD_DrawPoint ( x, y, 0X00); //画点
                }
                temp <<= 1;
                y++;
                if ((y - y1) == (8 - pos * 4)) {
                    y = y1;
                    x++;
                    break;
                }
            }
        }
    }
}
void LCD_Gram_ShowChar ( u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 size, u8 mode )
{
    u8 temp;
    u8 pos, j, t;
    u8 y0 = y;
    u8 y1;
    u8 x0 = x;
    if (size == 8) {
        LCD_Gram_ShowChar_Size0806( x, y, fc, bc, num, mode );
        return;
    } else if (size == 12) {
        LCD_Gram_ShowChar_Size1206( x, y, fc, bc, num, mode );
        return;
    } else if (size == 64) {
        num = num - '0';
    } else {
        num = num - ' '; //得到偏移后的值
    }
    for ( pos = 0; pos < (size + 4) / 8; pos++ ) {
        y1 = y0 + ((size + 4) / 8 - pos - 1) * 8;
        y = y1;
        x = x0;
        for ( t = 0; t < size / 2; t++ ) {
            if (size == 64)temp = asc_32_64[num][pos * 32 + t]; //调用6464字体
            if (size == 40)temp = asc_20_40[num][pos * 20 + t]; //调用6464字体
            else if ( size == 32 ) temp = asc_16_32[num][pos * 16 + t]; //调用3232字体
            else temp = asc2_1608[num][pos * 8 + t]; //调用1608字体
            if ( !mode ) { //非叠加方式
                temp = temp;
            } else {
                temp = ~temp;
            }
            for ( j = 0; j < 8 ; j++ ) {
                if ( temp & 0x80 ) {
                    pdev->LCD_DrawPoint ( x, y, 0XFF); //画点
                } else {
                    pdev->LCD_DrawPoint ( x, y, 0X00); //画点
                }
                temp <<= 1;
                y++;
                if ((y - y1) == 8) {
                    y = y1;
                    x++;
                    break;
                }
            }
        }
    }
}
#endif
void LCD_ShowString ( u16 x, u16 y, u8 size, u8 *p, u8 mode )
{
    while ( ( *p <= '~' ) && ( *p >= ' ' ) ) { //判断是不是非法字符!
        if ( x > ( pdev->width - 1 ) || y > ( pdev->height - 1 ) )
            return;

        LCD_ShowChar ( x, y, POINT_COLOR, BACK_COLOR, *p, size, mode );
        x += size / 2;
        p++;
    }
}

u32 mypow ( u8 m, u8 n )
{
    u32 result = 1;

    while ( n-- ) result *= m;

    return result;
}

void LCD_ShowNum ( u16 x, u16 y, u32 num, u8 len, u8 size )
{
    u8 t, temp;
    u8 enshow = 0;

    for ( t = 0; t < len; t++ ) {
        temp = ( num / mypow ( 10, len - t - 1 ) ) % 10;

        if ( enshow == 0 && t < ( len - 1 ) ) {
            if ( temp == 0 ) {
                LCD_ShowChar ( x + ( size / 2 ) *t, y, POINT_COLOR, BACK_COLOR, ' ', size, 0 );
                continue;
            } else enshow = 1;
        }

        LCD_ShowChar ( x + ( size / 2 ) *t, y, POINT_COLOR, BACK_COLOR, temp + '0', size, 0 );
    }
}

void GUI_DrawFont16 ( u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode )
{
    u8 i, pos, xtemp;
    u16 k;
    u16 HZnum;
//     u16 x0 = x;
    HZnum = sizeof ( tfont16 ) / sizeof ( typFNT_GB16 );    //自动统计汉字数目

    for ( k = 0; k < HZnum; k++ ) {
        if ( ( tfont16[k].Index[0] == * ( s ) ) && ( tfont16[k].Index[1] == * ( s + 1 ) ) ) {
            break;
        }
    }
    if (k == HZnum) { //没找到汉字，返回
        return;
    }
    pdev->LCD_Set_Window ( x, y, x + 16 - 1, y + 16 / 8 - 1 );
    for ( pos = 0; pos < 16 / 8; pos++ ) {
        if (target_is_watch_meter()) {
            pdev->LCD_WR_REG(0Xb0 + pos + y); //lcd12864没有窗口设置，需要手动调整坐标
            xtemp = x + 4; //反屏使用，需要调整X轴四个点，因为屏的线是132
            pdev->LCD_WR_REG(0x10 + xtemp / 16);
            pdev->LCD_WR_REG(0x00 + xtemp % 16);
        } else {
            pdev->LCD_WR_REG(0Xb4 + pos + y); //lcd12896没有窗口设置，需要手动调整坐标
            pdev->LCD_WR_REG(0x10 + x / 16);
            pdev->LCD_WR_REG(0x00 + x % 16);
        }
        for ( i = 0; i < 16 ; i++ ) {
            if ( !mode ) { //非叠加方式
//            pdev->LCD_WriteReg(0x01, tfont16[k].Msk[i]);
                pdev->LCD_WR_DATA(tfont16[k].Msk[i + pos * 16]);
                if (!target_is_watch_meter()) { //12896是四级灰度，需要写两遍
                    pdev->LCD_WR_DATA(tfont16[k].Msk[i + pos * 16]);
                }
            } else {
//            pdev->LCD_WriteReg(0x01, ~tfont16[k].Msk[i]);
                pdev->LCD_WR_DATA(~tfont16[k].Msk[i + pos * 16]);
                if (!target_is_watch_meter()) { //12896是四级灰度，需要写两遍
                    pdev->LCD_WR_DATA(~tfont16[k].Msk[i + pos * 16]);
                }
            }
        }
    }
    pdev->LCD_Set_Window ( 0, 0, pdev->width - 1, pdev->height - 1 ); //恢复窗口为全屏
}
void Gram_GUI_DrawFont16 ( u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode )
{
    u8 i;
    u16 j, k;
    u16 HZnum;
    u8 temp;
    u16 y1 = y, x0 = x;

    HZnum = sizeof ( tfont16 ) / sizeof ( typFNT_GB16 );    //自动统计汉字数目

    for ( k = 0; k < HZnum; k++ ) {
        if ( ( tfont16[k].Index[0] == * ( s ) ) && ( tfont16[k].Index[1] == * ( s + 1 ) ) ) {
            break;
        }
    }
    if (k == HZnum) { //没找到汉字，返回
        return;
    }
    for ( i = 0; i < 16 * 2; i++ ) {
        if ( !mode ) { //非叠加方式
            temp = tfont16[k].Msk[(i + 16) % 32];
        } else {
            temp = ~tfont16[k].Msk[(i + 16) % 32];
        }
        if (x - x0 == 16) {
            y1 += 8;
            y = y1;
            x = x0;
        }
        for ( j = 0; j < 8 ; j++ ) {
            if ( temp & 0x80 ) {
                pdev->LCD_DrawPoint ( x, y, 0XFF); //画点
            } else {
                pdev->LCD_DrawPoint ( x, y, 0X00); //画点
            }
            temp <<= 1;
            y++;
            if ((y - y1) == 8) {
                y = y1;
                x++;
                break;
            }
        }
    }
}
void GUI_DrawFont24 ( u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode )
{
#if LCD_TFT1==1
    u8 j;
    u16 x0 = x;
#endif
    u8 i, pos, xtemp;
    u16 k;
    u16 HZnum;
    HZnum = sizeof ( tfont24 ) / sizeof ( typFNT_GB24 );    //自动统计汉字数目

    for ( k = 0; k < HZnum; k++ ) {
        if ( ( tfont24[k].Index[0] == * ( s ) ) && ( tfont24[k].Index[1] == * ( s + 1 ) ) ) {
            break;
        }
    }
    if (k == HZnum) { //没找到汉字，返回
        return;
    }
    pdev->LCD_Set_Window ( x, y, x + 24 - 1, y + 24 / 8 - 1 );
    for ( pos = 0; pos < 24 / 8; pos++ ) {
#if LCD_TFT1==1
        for ( j = 0; j < 8; j++ ) {
            if ( !mode ) { //非叠加方式
                if ( tfont24[k].Msk[i] & ( 0x80 >> j ) ) {
                    pdev->LCD_DrawPoint_16Bit ( fc );
                } else {
                    pdev->LCD_DrawPoint_16Bit ( bc );
                }
            } else {
                POINT_COLOR = fc;
                if ( tfont24[k].Msk[i] & ( 0x80 >> j ) ) {
                    pdev->LCD_DrawPoint ( x, y );

                }
                x++;
                if ( ( x - x0 ) == 24 ) {
                    x = x0;
                    y++;
                    break;
                }
            }
        }
#else
        if (target_is_watch_meter()) {
            pdev->LCD_WR_REG(0Xb0 + pos + y); //lcd12864没有窗口设置，需要手动调整坐标
            xtemp = x + 4; //反屏使用，需要调整X轴四个点，因为屏的线是132
            pdev->LCD_WR_REG(0x10 + xtemp / 16);
            pdev->LCD_WR_REG(0x00 + xtemp % 16);
        } else {
            pdev->LCD_WR_REG(0Xb4 + pos + y); //lcd12896没有窗口设置，需要手动调整坐标
            pdev->LCD_WR_REG(0x10 + x / 16);
            pdev->LCD_WR_REG(0x00 + x % 16);
        }
        for ( i = 0; i < 24 ; i++ ) {
            if ( !mode ) { //非叠加方式
//            pdev->LCD_WriteReg(0x01, tfont16[k].Msk[i]);
                pdev->LCD_WR_DATA(tfont24[k].Msk[i + pos * 24]);
                if (!target_is_watch_meter()) { //12896是四级灰度，需要写两遍
                    pdev->LCD_WR_DATA(tfont24[k].Msk[i + pos * 24]);
                }
            } else {
//            pdev->LCD_WriteReg(0x01, ~tfont16[k].Msk[i]);
                pdev->LCD_WR_DATA(~tfont24[k].Msk[i + pos * 24]);
                if (!target_is_watch_meter()) { //12896是四级灰度，需要写两遍
                    pdev->LCD_WR_DATA(~tfont24[k].Msk[i + pos * 24]);
                }
            }
        }
#endif
    }
    pdev->LCD_Set_Window ( 0, 0, pdev->width - 1, pdev->height - 1 ); //恢复窗口为全屏
}

void GUI_DrawFont32 ( u16 x, u16 y, u16 fc, u16 bc, u8 *s, u8 mode )
{
    u8 i, pos, xtemp;
    u16 k;
    u16 HZnum;
//     u16 x0 = x;

    HZnum = sizeof ( tfont32 ) / sizeof (typFNT_GB32);  //自动统计汉字数目

    for ( k = 0; k < HZnum; k++ ) {
        if ( ( tfont32[k].Index[0] == * ( s ) ) && ( tfont32[k].Index[1] == * ( s + 1 ) ) ) {
            break;
        }
    }
    if (k == HZnum) { //没找到汉字，返回
        return;
    }
    pdev->LCD_Set_Window ( x, y, x + 32 - 1, y + 32 / 8 - 1 );
    for ( pos = 0; pos < 32 / 8; pos++ ) {
        if (target_is_watch_meter()) {
            pdev->LCD_WR_REG(0Xb0 + pos + y); //lcd12864没有窗口设置，需要手动调整坐标
            xtemp = x + 4; //反屏使用，需要调整X轴四个点，因为屏的线是132
            pdev->LCD_WR_REG(0x10 + xtemp / 16);
            pdev->LCD_WR_REG(0x00 + xtemp % 16);
        } else {
            pdev->LCD_WR_REG(0Xb4 + pos + y); //lcd12896没有窗口设置，需要手动调整坐标
            pdev->LCD_WR_REG(0x10 + x / 16);
            pdev->LCD_WR_REG(0x00 + x % 16);
        }
        for ( i = 0; i < 32 ; i++ ) {
            if ( !mode ) { //非叠加方式
//            pdev->LCD_WriteReg(0x01, tfont16[k].Msk[i]);
                pdev->LCD_WR_DATA(tfont32[k].Msk[i + pos * 32]);
                if (!target_is_watch_meter()) { //12896是四级灰度，需要写两遍
                    pdev->LCD_WR_DATA(tfont32[k].Msk[i + pos * 32]);
                }
            } else {
//            pdev->LCD_WriteReg(0x01, ~tfont16[k].Msk[i]);
                pdev->LCD_WR_DATA(~tfont32[k].Msk[i + pos * 32]);
                if (!target_is_watch_meter()) { //12896是四级灰度，需要写两遍
                    pdev->LCD_WR_DATA(~tfont32[k].Msk[i + pos * 32]);
                }
            }
        }
    }

    pdev->LCD_Set_Window ( 0, 0, pdev->width - 1, pdev->height - 1 ); //恢复窗口为全屏
}

void Show_Str ( u16 x, u16 y, u16 fc, u16 bc, u8 *str, u8 size, u8 mode )
{
    u16 x0 = x;
    u8 bHz = 0;   //字符或者中文

    while ( *str != 0 ) { //数据未结束
        if ( !bHz ) {
            if ( x > ( pdev->width - size / 2 ) || y > ( pdev->height - size ) )
                return;

            if ( *str > 0x80 ) bHz = 1; //中文
            else {            //字符
                if ( *str == 0x0A ) { //换行符号
                    y += size;
                    x = x0;
                    str++;
                } else {
                    LCD_ShowChar ( x, y, fc, bc, *str, size, mode );
                    if (size == 8) {
                        x += 6;     //  0806的字体，特殊处理
                    } else {
                        if (size <= 40) {
                            if (*str == '.') {
                                x += size / 2 - (size / 16 * 4);
                            } else {
                                x += size / 2; //字符,为全字的一半
                            }
                        } else {
                            if (*str == '<') {
                                x += size / 2 - (size / 16 * 4);
                            } else {
                                x += size / 2; //字符,为全字的一半
                            }
                        }
                    }
                }
                str++;
            }
        } else { //中文
            if ( x > ( pdev->width - size ) || y > ( pdev->height - size ) )
                return;

            bHz = 0; //有汉字库

            if ( size == 32 )
                GUI_DrawFont32 ( x, y, fc, bc, str, mode );
            else if ( size == 24 )
                GUI_DrawFont24 ( x, y, fc, bc, str, mode );
            else
                GUI_DrawFont16 ( x, y, fc, bc, str, mode );

            str += 2;
            x += size; //下一个汉字偏移
        }
    }
}

void Gram_Show_Str ( u16 x, u16 y, u16 fc, u16 bc, u8 *str, u8 size, u8 mode )
{
    u16 x0 = x;
    u8 bHz = 0;   //字符或者中文

    while ( *str != 0 ) { //数据未结束
        if ( !bHz ) {
            if ( x > ( pdev->width - size / 2 ) || y > ( 127 - size ) )
                return;

            if ( *str > 0x80 ) bHz = 1; //中文
            else {            //字符
                if ( *str == 0x0D ) { //换行符号
                    y += size;
                    x = x0;
                    str++;
                } else {
                    LCD_Gram_ShowChar ( x, y, fc, bc, *str, size, mode);
                    if (size == 8) {
                        x += 6; //0806字符,宽度特殊
                    } else {
                        x += size / 2; //字符,为全字的一半
                    }
                }
                str++;
            }
        } else { //中文
            if ( x > ( pdev->width - size / 2 ) || y > ( 127 - size ) )
                return;
            bHz = 0; //有汉字库

            if ( size == 32 )
                GUI_DrawFont32 ( x, y, fc, bc, str, mode );
            else if ( size == 24 )
                GUI_DrawFont24 ( x, y, fc, bc, str, mode );
            else
                Gram_GUI_DrawFont16 ( x, y, fc, bc, str, mode );

            str += 2;
            x += size; //下一个汉字偏移
        }
    }
}
void Gui_StrCenter ( u16 x, u16 y, u16 fc, u16 bc, u8 *str, u8 size, u8 mode )
{
    u16 len = strlen ( ( const char * ) str );
    u16 x1 = ( pdev->width - len * 8 ) / 2;
    Show_Str ( x + x1, y, fc, bc, str, size, mode );
}
void Gui_Drawbmp20 ( u16 x, u16 y, const unsigned char *p ) //显示20*25 usb标志图片
{
    int i;
    unsigned char picH, picL;
    pdev->LCD_Set_Window ( x, y, x + 20 - 1, y + 25 - 1 ); //窗口设置

    for ( i = 0; i < 20 * 25; i++ ) {
        picL = * ( p + i * 2 ); //数据低位在前
        picH = * ( p + i * 2 + 1 );
//        pdev->LCD_DrawPoint_16Bit ( picH << 8 | picL );
    }

    pdev->LCD_Set_Window ( 0, 0, pdev->width - 1, pdev->height - 1 ); //恢复显示窗口为全屏
}
void Gui_Drawbmp16 ( u16 x, u16 y, const unsigned char *p ) //显示40*40 QQ图片
{
    int i;
    unsigned char picH, picL;
    pdev->LCD_Set_Window ( x, y, x + 40 - 1, y + 40 - 1 ); //窗口设置

    for ( i = 0; i < 40 * 40; i++ ) {
        picL = * ( p + i * 2 ); //数据低位在前
        picH = * ( p + i * 2 + 1 );
//        pdev->LCD_DrawPoint_16Bit ( picH << 8 | picL );
    }

    pdev->LCD_Set_Window ( 0, 0, pdev->width - 1, pdev->height - 1 ); //恢复显示窗口为全屏
}

void Gui_Lcd_Clear(u16 color)
{
    pdev->LCD_Clear(color);
}

void Gui_Show_Normal(void)
{

}

void register_lcd_dev ( _lcd_dev *p )
{
    pdev = p;
}

void Gui_Lcd_Unit(ModifyDataType mdata, u8 size)
{
    char strtemp;
    u8 i;
    if (size == 64) {
        GUI_LCD_Fill( 0, 160, 320, 240, BLUE);
        for (i = 0; i < 3; i++) {
            strtemp = (char) * (mdata.unitstringList + i);
            if (strtemp == 'K') {
                strtemp = '=';
            } else if (strtemp == 'g') {
                strtemp = '>';
            } else if (strtemp == 't') {
                strtemp = '?';
            } else if (strtemp == 'P') {
                strtemp = '@';
            } else if (strtemp == 'a') {
                strtemp = 'A';
            } else if ((strtemp < '0') || (strtemp > '9')) {
                strtemp = ':';
            }
            LCD_ShowChar (224 + 32 * i, 168, RED, BLUE, strtemp, 64, 0);
        }
    } else if (size == 32) {
        for (i = 0; i < 4; i++) {
            strtemp = (char) * (mdata.unitstringList + i);
            LCD_ShowChar (160 + 16 * i, 8, RED, BLACK, strtemp, 32, 0);
        }
    }
}
//LCD显示电池电量标志
//sx,sx起点坐标
// batpercent电量百分比
//consumecolor消耗电量的颜色
//batcolor电池标志的颜色
void Gui_Lcd_Battery(u16 sx, u16 sy, u8 batpercent, u16 consumecolor, u16 batcolor)
{
    //只需要起始坐标，宽度和长度都固定
    u16 i, j;
    u16 exconsume, eyconsume, xconsumelen, xbatlen;
#if LCD_TFT1==1
    static u8 width = 24, lenth = 50;

    if (batpercent > 100) {
        batpercent = 100;
    }
//     xconsumelen=(100-batpercent)*lenth/100;
    xconsumelen = batpercent * lenth / 100; //剩余电量和消耗电量对调
//     xbatlen=lenth-xconsumelen;
    exconsume = xconsumelen + sx; //消耗的终点也就是电池剩余的起点
//     eyconsume=width+sy;              //宽度是固定的
//     LCD_DrawRectangle (sx,sy,sx+lenth+2,sy+width+2,consumecolor);
    pdev->LCD_Set_Window(sx, sy, exconsume, eyconsume); //设置光标位置
    for (i = sy; i <= width + sy; i++) {
        pdev->LCD_SetCursor(sx, i);                     //设置光标位置
        pdev->LCD_WriteRAM_Prepare();               //开始写入GRAM
        for (j = 0; j < xconsumelen; j++) pdev->LCD_DrawPoint_16Bit(batcolor); //设置光标位置
    }

    pdev->LCD_Set_Window(exconsume, sy, lenth + sx, eyconsume + 1); //设置光标位置
    for (i = sy; i <= width + sy; i++) {
        pdev->LCD_SetCursor(exconsume, i);                  //设置光标位置
        pdev->LCD_WriteRAM_Prepare();               //开始写入GRAM
        for (j = 0; j < xbatlen; j++) pdev->LCD_DrawPoint_16Bit(consumecolor); //设置光标位置
    }

    pdev->LCD_Set_Window(lenth + sx, ((sy + width / 2) - 4), lenth + sx + 4, ((sy + width / 2) + 4)); //设置光标位置
    for (i = ((sy + width / 2) - 4); i <= ((sy + width / 2) + 4); i++) {
        pdev->LCD_SetCursor(lenth + sx, i);                 //设置光标位置
        pdev->LCD_WriteRAM_Prepare();               //开始写入GRAM
        for (j = 0; j < 4; j++) pdev->LCD_DrawPoint_16Bit(consumecolor); //设置光标位置
    }
#else
    static u8 width = 2, lenth = 30;

    if (batpercent > 100) {
        batpercent = 100;
    }
//     xconsumelen=(100-batpercent)*lenth/100;
    xconsumelen = batpercent * lenth / 100; //剩余电量和消耗电量对调
    xbatlen = lenth - xconsumelen;
    exconsume = xconsumelen + sx; //消耗的终点也就是电池剩余的起点
    eyconsume = width + sy;         //宽度是固定的
    eyconsume = eyconsume;
    xbatlen = xbatlen;
    pdev->LCD_Set_Window(sx - 2, sy, sx - 1, sy + width - 1); //设置光标位置
    pdev->LCD_WriteReg(0x01, 0xf0);
    pdev->LCD_WriteReg(0x01, 0xf0);
    pdev->LCD_WriteReg(0x01, 0x0f);
    pdev->LCD_WriteReg(0x01, 0x0f);
    pdev->LCD_Set_Window(sx, sy, lenth + sx - 1, sy + width - 1); //设置光标位置
    for (i = sy; i < width + sy; i++) {
        for (j = sx; j < sx + lenth; j++) {
            if ((j == sx) || (j == sx + lenth - 1)) {
                pdev->LCD_WriteReg(0x01, 0xFF);
            } else if ((j == sx + 1) || (j == sx + lenth - 2)) {
                if (i == sy) {
                    pdev->LCD_WriteReg(0x01, 0x01);
                } else {
                    pdev->LCD_WriteReg(0x01, 0x80);
                }
            } else {
                if (i == sy) {
                    if (j < exconsume - 2) {
                        pdev->LCD_WriteReg(0x01, 0x01);
                    } else {
                        pdev->LCD_WriteReg(0x01, 0xFD);
                    }
                } else {
                    if (j < exconsume - 2) {
                        pdev->LCD_WriteReg(0x01, 0x80);
                    } else {
                        pdev->LCD_WriteReg(0x01, 0xBF);
                    }
                }

            }
        }
    }

#endif

}
void Gui_clear_battery(u16 sx, u16 sy, u16 width, u16 lenth)
{
    u16 j;
    pdev->LCD_Set_Window(sx, sy, sx + lenth, sy + width); //设置光标位置
    for (j = 0; j < width * lenth; j++) {
        pdev->LCD_WriteReg(0x01, 0x00);
    }
}
void Gui_battery_badstate(u16 sx, u16 sy)
{
    const unsigned char battery_bad_map[30 * 2] = {
        0xFF, 0x01, 0x03, 0x07, 0x07, 0x0D, 0x09, 0x19, 0x11, 0x31, 0x61, 0x61, 0xC1, 0xC1, 0x81,
        0x81, 0x81, 0xC1, 0x41, 0x61, 0x21, 0x31, 0x11, 0x19, 0x09, 0x0D, 0x05, 0x07, 0x03, 0xFF,
        0xFF, 0xC0, 0xC0, 0xE0, 0xE0, 0xB0, 0x90, 0x98, 0x88, 0x8C, 0x8C, 0x86, 0x86, 0x83, 0x83,
        0x81, 0x81, 0x83, 0x86, 0x86, 0x8C, 0x88, 0x88, 0x98, 0xB0, 0xB0, 0xE0, 0xE0, 0xC0, 0xFF, /*"未命名文件",0*/
    };
    u16 i, j;
    static u8 width = 2, lenth = 30;

    pdev->LCD_Set_Window(sx - 2, sy, sx - 1, sy + width - 1); //设置光标位置
    pdev->LCD_WriteReg(0x01, 0xf0);
    pdev->LCD_WriteReg(0x01, 0xf0);
    pdev->LCD_WriteReg(0x01, 0x0f);
    pdev->LCD_WriteReg(0x01, 0x0f);
    pdev->LCD_Set_Window(sx, sy, lenth + sx - 1, sy + width - 1); //设置光标位置
    for (i = 0; i < width ; i++) {
        for (j = 0; j < lenth; j++) {
            pdev->LCD_WriteReg(0x01, battery_bad_map[j + i * lenth]);
        }
    }
}
// void Gui_Usb_Icon(void)
// {
//     if (bDeviceState == 0x05){
//         usb_displaystate++;
//         if(usb_displaystate == 1){
//              Gui_Drawbmp20 (226+3,3,gImage_usb_icon);
//         }else{
//            usb_displaystate = 2;
//         }
//     }else{
//         if(usb_displaystate){
//             GUI_LCD_Fill(226+3,3,226+20+3,3+25,GREEN);
//             usb_displaystate = 0;
//         }
//     }
// }
extern int check_vbus(void);
void Gui_Lcd_Interface(void)
{
//    if (target_is_watch_meter()) {
//        return;
//    }
    GUI_LCD_Fill( 0, 0, 480, 32, GREEN);
    GUI_LCD_Fill( 0, 32, 480, 400, BLACK);
//    if (check_vbus()) {
//        Show_Str( 10, 3, BLUE, WHITE, "Loading", LCDsize24, 0);
//    }
//     GUI_LCD_Fill( 0,208,320,240,WHITE);
}

void Gui_display_error(u16 x, u16 y, Display_Blink_Type blinktype, u8 size, u8 mode)
{
    u8 i;
    for (i = 0; i < 5; i++) {
        switch (blinktype) {
            case EEPROM_ERR:
                Show_Str(x, y, RED, WHITE, "ERROR 00", size, mode);
                break;
            case ADC_ERR:
                Show_Str(x, y, RED, WHITE, "ERROR 01", size, mode);
                break;
            case CALIBRA_ERR:
                Show_Str(x, y, RED, WHITE, "calibErr", size, mode);
                break;
            case PARAM_ERR:
                Show_Str(x, y, RED, WHITE, "ParamErr", size, mode);
                break;
            case PASSWORD_ERR:
                Show_Str(x, y, RED, WHITE, "ERROR 04", size, mode);
                break;
            case PASSWORD_OK:
                Show_Str(x, y, RED, WHITE, "PASSW OK", size, mode);
                break;
            case CALIBRA_OK:
                Show_Str(x, y, RED, WHITE, "calib OK", size, mode);
                break;
            case SDCARD_ERR:
                Show_Str(x, y, RED, WHITE, "ERROR 05", size, mode);
                break;
            case CHKSUM_ERR:
                Show_Str(x, y, RED, WHITE, "ERROR 06", size, mode);
                break;
            default:
                break;
        }
        Delay(500);
        Show_Str(x, y, RED, WHITE, "        ", size, 0);
        Delay(300);
    }
    switch (blinktype) {
        case EEPROM_ERR:
            Show_Str(x, y, RED, WHITE, "ERROR 00", size, mode);
            break;
        case ADC_ERR:
            Show_Str(x, y, RED, WHITE, "ERROR 01", size, mode);
            break;
        case CALIBRA_ERR:
            Show_Str(x, y, RED, WHITE, "calibErr", size, mode);
            break;
        case PARAM_ERR:
            Show_Str(x, y, RED, WHITE, "ParamErr", size, mode);
            break;
        case PASSWORD_ERR:
            Show_Str(x, y, RED, WHITE, "ERROR 04", size, mode);
            break;
        case PASSWORD_OK:
            Show_Str(x, y, RED, WHITE, "PASSW OK", size, mode);
            break;
        case CALIBRA_OK:
            Show_Str(x, y, RED, WHITE, "calib OK", size, mode);
            break;
        case SDCARD_ERR:
            Show_Str(x, y, RED, WHITE, "ERROR 05", size, mode);
            break;
        case CHKSUM_ERR:
            Show_Str(x, y, RED, WHITE, "ERROR 06", size, mode);
            break;
        default:
            break;
    }
}

/*********************LCD画点************************************/
//更新显存到LCD
void Gui_Refresh_Gram(void)
{
    u16 n;
    static u8 enum_update_gram = 0;
//    pdev->LCD_Set_Window ( 0, enum_update_gram, pdev->width - 1, enum_update_gram); //恢复显示窗口为全屏
    pdev->LCD_WR_REG(0xb4 + enum_update_gram);
    pdev->LCD_WR_REG(0x10);
    pdev->LCD_WR_REG(0x00);
    for (n = 0; n < 128; n++) {
//        pdev->LCD_WriteReg(0x01, Curve.LCD_GRAM[n][enum_update_gram]);
        pdev->LCD_WR_DATA(Curve.LCD_GRAM[n][enum_update_gram]);
        pdev->LCD_WR_DATA(Curve.LCD_GRAM[n][enum_update_gram]);
    }
    enum_update_gram++;
    if (enum_update_gram >= 12) {
        enum_update_gram = 0;
    }
}
// //更新显存到LCD
void Gui_Refresh_AllGram(void)
{
    u16 n, i;

    for (i = 0; i < 12; i++) {
        pdev->LCD_WR_REG(0xb4 + i);
        pdev->LCD_WR_REG(0x10);
        pdev->LCD_WR_REG(0x00);
        for (n = 0; n < 128; n++) {
            pdev->LCD_WR_DATA(Curve.LCD_GRAM[n][i]);
            pdev->LCD_WR_DATA(Curve.LCD_GRAM[n][i]);
        }
    }
}
void Gui_Clear_Gram(void)
{
    u8 i, n;
    for (i = 0; i < 12; i++) {
        for (n = 0; n < 128; n++) {
            Curve.LCD_GRAM[n][i] = 0X00;
        }
    }
}
void Gui_Control_Contrast(u8 percent)
{
//    pdev->LCD_Control_Contrast(percent);
}
void Gui_Deinit_Lcd(void)
{
    pdev->LCD_DisplayOff();
}
void Gui_Curve(Curve_Typde *curve, u8 gui_channal)
{
#define PPMV_CHANNAL  3
    u8 i, j, curce_dot;
    u8 k;
    u8* dispaly_curvebuf;
    static long curve_old_dot_scale = 1;
    float curve_y;
    //清理所有内存后重新描绘
    Gui_Clear_Gram();

    if (curve_old_dot_scale != get_dewpoint_scale()) {
        for (j = 0; j < PPMV_CHANNAL; j++) { //PPMV固定小数点1位，不需要转化
            for (i = 0; i <= Curve.curve_num[j]; i++) {
                if (get_dewpoint_scale() > curve_old_dot_scale) {
                    Curve.curve_pv[j][i].i = Curve.curve_pv[j][i].i*\
                                             (get_dewpoint_scale() / curve_old_dot_scale);
                } else {
                    Curve.curve_pv[j][i].i = Curve.curve_pv[j][i].i / \
                                             (curve_old_dot_scale / get_dewpoint_scale());
                }
            }
        }
        for (j = 0; j < 3; j++) { //PPMV固定小数点1位，不需要转化
            if (get_dewpoint_scale() > curve_old_dot_scale) {
                Curve.curve_pv_min[j].i = Curve.curve_pv_min[j].i*\
                                          (get_dewpoint_scale() / curve_old_dot_scale);
                Curve.curve_pv_max[j].i = Curve.curve_pv_max[j].i*\
                                          (get_dewpoint_scale() / curve_old_dot_scale);

            } else {
                Curve.curve_pv_min[j].i = Curve.curve_pv_min[j].i / \
                                          (curve_old_dot_scale / get_dewpoint_scale());
                Curve.curve_pv_max[j].i = Curve.curve_pv_max[j].i / \
                                          (curve_old_dot_scale / get_dewpoint_scale());
            }
        }
    }
    LCD_DrawLine (CURVE_X1 - 1, CURVE_Y1 - 1, CURVE_X1 - 1, CURVE_Y2 + 1, WHITE); //画纵坐标
    LCD_DrawLine (CURVE_X1 - 1, CURVE_Y1 - 1, CURVE_X2 - 1, CURVE_Y1 - 1, WHITE); //画横坐标
    for (i = CURVE_Y1 + 5; i <= CURVE_Y2; i += 5) {
        if (i % 10) {
            LCD_DrawLine (0, i - 1, 4, i - 1, WHITE);
        } else {
            LCD_DrawLine (2, i - 1, 4, i - 1, WHITE);
        }
    }
    for (i = CURVE_X1 + 5; i <= CURVE_X2; i += 5) {
        if (i % 10) {
            LCD_DrawLine (i - 1, CURVE_Y1 - 5, i - 1, CURVE_Y1 - 1, WHITE);
        } else {
            LCD_DrawLine (i - 1, CURVE_Y1 - 3, i - 1, CURVE_Y1 - 1, WHITE);
        }
    }
    curve_old_dot_scale = get_dewpoint_scale();
    if (curve->curve_mode <= Y_MAN_LINE_POINT) { //曲线Y轴手动设置模式
        if (((param.graph_y2.l == 0) && (param.graph_y1.l == 0)) || (param.graph_y1.l >= param.graph_y2.l)) {
            curve_y = 0;
        } else {
            curve_y = ((float)(CURVE_Y2 - CURVE_Y1)) / (param.graph_y2.l - param.graph_y1.l); //计算Y轴的K值
        }
        for (i = 0; i < curve->curve_num[gui_channal]; i++) {//为了保留真实最大值最小值给客户显示，但是超出后曲线显示极限值
            if (curve->curve_pv[gui_channal][i].i > param.graph_y2.l) {
                Curve.curve_xy[i] = (u8)(curve_y * (param.graph_y2.l - param.graph_y1.l)) + CURVE_Y1;
            } else if (curve->curve_pv[gui_channal][i].i < param.graph_y1.l) {
                Curve.curve_xy[i] = (u8)(curve_y * (param.graph_y1.l - param.graph_y1.l)) + CURVE_Y1;
            } else {
                Curve.curve_xy[i] = (u8)(curve_y * (curve->curve_pv[gui_channal][i].i - param.graph_y1.l)) + CURVE_Y1;
            }
        }//确定坐标值
    } else { //曲线Y轴自动设置模式，不处理即可
        if (((curve->curve_pv_max[gui_channal].i == 0) && (curve->curve_pv_min[gui_channal].i == 0)) || \
            (curve->curve_pv_min[gui_channal].i >= curve->curve_pv_max[gui_channal].i)) {
            curve_y = 0;
        } else {
            curve_y = ((float)(CURVE_Y2 - CURVE_Y1)) / (curve->curve_pv_max[gui_channal].i - curve->curve_pv_min[gui_channal].i); //计算Y轴的K值
        }
        for (i = 0; i < curve->curve_num[gui_channal]; i++) {
            Curve.curve_xy[i] = (u8)(curve_y * (curve->curve_pv[gui_channal][i].i - curve->curve_pv_min[gui_channal].i)) + CURVE_Y1;
        }//确定坐标值
    }
    if (curve->curve_num[gui_channal] >= 2) {
        for (i = 0; i < curve->curve_num[gui_channal] - 1; i++) {
            LCD_DrawLine (CURVE_X1 + (i) * 5, curve->curve_xy[i], CURVE_X1 + (i + 1) * 5, curve->curve_xy[i + 1], WHITE);
            if ((curve->curve_mode == Y_MAN_CIRCLE_POINT) || (curve->curve_mode == Y_AUTO_CIRCLE_POINT)) {
                gui_circle( CURVE_X1 + (i) * 5, curve->curve_xy[i], 1, 1, WHITE);
                gui_circle ( CURVE_X1 + (i + 1) * 5, curve->curve_xy[i + 1], 1, 1, WHITE);
            }
        }//绘制曲线
    } else if (curve->curve_num[gui_channal] == 1) {
        pdev->LCD_DrawPoint((u16)CURVE_X1, (u16)(curve->curve_xy[0]), (u8)WHITE); //画点
    } else {
        return;
    }
    Gram_Show_Str( 0, 0, RED, WHITE, "MAX:", LCDsize8, 0);
    Gram_Show_Str( 66, 0, RED, WHITE, "MIN:", LCDsize8, 0);
    Gram_Show_Str ( 0, 10, RED, WHITE, "CURRENT:", LCDsize8, 0);
    curce_dot = (u8)param.dot_num.l;
    if (gui_channal == PPMV_CHANNAL) { //PPMV
        curce_dot = 0;
        dprintf_om(curve->curve_pv_max[gui_channal].i / 10, curce_dot, 24, 0, 0, LCDsize8, showleft);
        dprintf_om(curve->curve_pv_min[gui_channal].i / 10, curce_dot, 66 + 24, 0, 0, LCDsize8, showleft);
        k = dprintf_om(curve->curve_pv[gui_channal][curve->curve_num[gui_channal] - 1].i / 10, curce_dot, 48, 10, 0, LCDsize8, showleft);
    } else {
        dprintf_om(curve->curve_pv_max[gui_channal].i, curce_dot, 24, 0, 0, LCDsize8, showleft);
        dprintf_om(curve->curve_pv_min[gui_channal].i, curce_dot, 66 + 24, 0, 0, LCDsize8, showleft);
        k = dprintf_om(curve->curve_pv[gui_channal][curve->curve_num[gui_channal] - 1].i, curce_dot, 48, 10, 0, LCDsize8, showleft);
    }
    if (gui_channal == 0) { //显示temp单位
        dispaly_curvebuf = param.unit_name.l ? (u8 *)("~F  ") : (u8 *)("~C  ");
        Gram_Show_Str ( 48 + (k + 1) * 6, 10, RED, WHITE, (u8*)dispaly_curvebuf, LCDsize8, 0);
    } else if (gui_channal == 1) { //显示humi单位
        Gram_Show_Str ( 48 + (k + 1) * 6, 10, RED, WHITE, "%RH ", LCDsize8, 0);
    } else if (gui_channal == 2) { //显示dew单位
        dispaly_curvebuf = param.unit_name.l ? (u8 *)("Dp~F") : (u8 *)("Dp~C");
        Gram_Show_Str ( 48 + (k + 1) * 6, 10, RED, WHITE, (u8*)dispaly_curvebuf, LCDsize8, 0);
    } else if (gui_channal == 3) { //显示ppmv单位
        Gram_Show_Str ( 48 + (k + 1) * 6, 10, RED, WHITE, "PPM ", LCDsize8, 0);
    }

}

void Display_Sub_Param(u16 sx, u16 sy, u8 size, u8 select_index)
{
    TagRuntime* pruntime;
    pruntime = get_runtime_value();
    switch (select_index) {
        case 0:
            dprintf_om(pruntime->drew_value, (u8)param.dot_num.l, sx, sy, 0, size, showright);
            Show_Str(sx + size / 2 * 8, sy, BLUE, WHITE, "℃td ", size, 0);
            break;
        case 1:
            dprintf_om(pruntime->ppmv_value, (u8)param.dot_num.l, sx, sy, 0, size, showright);
            Show_Str(sx + size / 2 * 8, sy, BLUE, WHITE, "PPMV", size, 0);
            break;
        case 2:
            dprintf_om(pruntime->temp_value, (u8)param.dot_num.l, sx, sy, 0, size, showright);
            Show_Str(sx + size / 2 * 8, sy, BLUE, WHITE, "℃  ", size, 0);
            break;
        case 3:
            dprintf_om(pruntime->humi_value, (u8)param.dot_num.l, sx, sy, 0, size, showright);
            Show_Str(sx + size / 2 * 8, sy, BLUE, WHITE, "%RH ", size, 0);
            break;
        case 4:

            break;
        case 5:

            break;
        default:
            break;
    }
}

static void Display_DpWatch_Main_Param(void )
{
    u8* dispaly_buf;
    TagRuntime* pruntime;
    pruntime = get_runtime_value();
    static u8 olddetectstate = 0;
    u8 temp_disp_line, humi_disp_line, dew_disp_line, ppmv_disp_line;
    long dis_value_convert = 0;
    temp_disp_line = 1;
    humi_disp_line = 1;
    dew_disp_line = 1;
    ppmv_disp_line = 1;
    switch (pruntime->lenv_value) {
        case 2:
            break;
        case 4:
            switch (pruntime->display_index) {
                case 0:
                    dispaly_buf = param.unit_name.l ? (u8 *)("DP℉") : (u8 *)("DP℃");
//                    Show_Str( 88, 2, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    if (sensor_err_count < SENSOR_ERR_NUM ) {
                        if (dewoutofrangeFlag != 0) {
                            if (pruntime->drew_value > 0) {
                                Show_Str(0, dew_disp_line, BLUE, WHITE, " >20.0", LCDsize40, 0);
                            } else {
                                Show_Str(0, dew_disp_line, BLUE, WHITE, " <-80", LCDsize40, 0);
                            }
                            GUI_LCD_Fill(112, dew_disp_line, 120, dew_disp_line + 5, BLACK);
                            Show_Str( 64 + 16, dew_disp_line + 5, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize16, 0);
                            break;
                        }
                        dis_value_convert = (long long)pruntime->drew_value / (100 / get_dewpoint_scale());
                        if ((u8)param.dot_num.l != 0) {
                            GUI_LCD_Fill(112, dew_disp_line, 120, dew_disp_line + 5, BLACK);
                            if (((dis_value_convert < -9999) || (dis_value_convert > 99999)) && (param.dot_num.l == 2)) {
                                dprintf_om(dis_value_convert / 10, (u8)param.dot_num.l - 1, 0, dew_disp_line, 1, LCDsize40, showright);
                            } else {
                                dprintf_om(dis_value_convert, (u8)param.dot_num.l, 0, dew_disp_line, 1, LCDsize40, showright);
                            }
                        } else {
                            dprintf_om(dis_value_convert, (u8)param.dot_num.l, 0, dew_disp_line, 1, LCDsize40, showright);
                        }
                    } else {
                        Show_Str(0, dew_disp_line, BLUE, WHITE, " --.--", LCDsize40, 0);
                        GUI_LCD_Fill(112, dew_disp_line, 120, dew_disp_line + 5, BLACK);
                        SensorOnlineFlag = 0;
                    }
                    Show_Str( 64 + 16, dew_disp_line + 5, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize16, 0);
                    break;
                case 1:
                    dispaly_buf = param.unit_name.l ? (u8 *)(" ℉ ") : (u8 *)(" ℃ ");
//                               dispaly_buf = param.unit_name.l ? (u8 *)("DP℉") : (u8 *)("DP℃");
//                    Show_Str( 88, 2, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    dis_value_convert = (long long)pruntime->temp_value / (100 / get_dewpoint_scale());
                    if (sensor_err_count < SENSOR_ERR_NUM ) {
                        if ((u8)param.dot_num.l != 0) {
                            GUI_LCD_Fill(112, temp_disp_line, 120, temp_disp_line + 5, BLACK);
                            if (((dis_value_convert < -9999) || (dis_value_convert > 99999)) && (param.dot_num.l == 2)) {
                                dprintf_om(dis_value_convert / 10, (u8)param.dot_num.l - 1, 0, temp_disp_line, 1, LCDsize40, showright);
                            } else {
                                dprintf_om(dis_value_convert, (u8)param.dot_num.l, 0, temp_disp_line, 1, LCDsize40, showright);
                            }
                        } else {
                            dprintf_om(dis_value_convert, (u8)param.dot_num.l, 0, temp_disp_line, 1, LCDsize40, showright);

                        }
                    } else {
                        Show_Str(0, temp_disp_line, BLUE, WHITE, " --.--", LCDsize40, 0);
                        GUI_LCD_Fill(112, temp_disp_line, 120, temp_disp_line + 5, BLACK);
                    }
                    Show_Str( 64 + 16, temp_disp_line + 5, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize16, 0);
                    break;
                case 3:
//                    dispaly_buf = lang ? (u8 *)("湿度") : (u8 *)("HUMI");
//                    Show_Str( 88, 2, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    dis_value_convert = pruntime->humi_value / (100 / get_dewpoint_scale());
                    if (sensor_err_count < SENSOR_ERR_NUM ) {
                        if ((u8)param.dot_num.l != 0) {
                            GUI_LCD_Fill(112, humi_disp_line, 120, humi_disp_line + 5, BLACK);
                            dprintf_om(dis_value_convert, (u8)param.dot_num.l, 0, humi_disp_line, 1, LCDsize40, showright);
                        } else {
                            dprintf_om(dis_value_convert, (u8)param.dot_num.l, 0, humi_disp_line, 1, LCDsize40, showright);
                        }
                    } else {
                        Show_Str(0, humi_disp_line, BLUE, WHITE, " --.--", LCDsize40, 0);
                        GUI_LCD_Fill(112, humi_disp_line, 120, humi_disp_line + 5, BLACK);
                    }
                    Show_Str( 64 + 16, humi_disp_line + 5, BLUE, WHITE, "%RH ", LCDsize16, 0);
                    break;
                case 5:
                    dispaly_buf = param.unit_name.l ? (u8 *)("DP℉") : (u8 *)("DP℃");
//                    Show_Str( 88, 2, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    dis_value_convert = (long long)pruntime->drew_value / (100 / get_dewpoint_scale());
                    if (sensor_err_count < SENSOR_ERR_NUM ) {
                        if ((u8)param.dot_num.l != 0) {
                            GUI_LCD_Fill(112, temp_disp_line, 120, dew_disp_line + 5, BLACK);
                            if (((dis_value_convert < -9999) || (dis_value_convert > 99999)) && (param.dot_num.l == 2)) {
                                dprintf_om(dis_value_convert / 10, (u8)param.dot_num.l - 1, 0, dew_disp_line, 1, LCDsize40, showright);
                            } else {
                                dprintf_om(dis_value_convert, (u8)param.dot_num.l, 0, dew_disp_line, 1, LCDsize40, showright);
                            }
                        } else {
                            dprintf_om(dis_value_convert, (u8)param.dot_num.l, 0, dew_disp_line, 1, LCDsize40, showright);
                        }
                    } else {
                        Show_Str(0, dew_disp_line, BLUE, WHITE, " --.--", LCDsize40, 0);
                        GUI_LCD_Fill(112, dew_disp_line, 120, dew_disp_line + 5, BLACK);
                    }
                    Show_Str( 64 + 16, dew_disp_line + 5, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize16, 0);
                    break;
                case 7:
//                    dispaly_buf = lang ? (u8 *)("微水") : (u8 *)("PPMV");
//                    Show_Str(88, 2, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    if (sensor_err_count < SENSOR_ERR_NUM ) {
                        if (pruntime->ppmv_value > 9999999) {
                            Show_Str(0, ppmv_disp_line, BLUE, WHITE, "HHHHHH", LCDsize40, 0);
                        } else {
                            dprintf_om(pruntime->ppmv_value / 10, 0, 0, ppmv_disp_line, 1, LCDsize40, showright);
                        }
                    } else {
                        Show_Str(0, ppmv_disp_line, BLUE, WHITE, " --.--", LCDsize40, 0);
                        GUI_LCD_Fill(112, ppmv_disp_line, 120, ppmv_disp_line + 5, BLACK);
                    }
                    Show_Str( 64 + 16, ppmv_disp_line + 5, BLUE, WHITE, "PPM ", LCDsize16, 0);
                    break;
                case 2:
                case 4:
                case 6:
                case 8:
                    break;
                default:
                    break;
            }
        default:
            break;
    }
    if (TimeInterval != 0) {
        if ((4 == pruntime->lenv_value) && (pruntime->display_index > 0)) {
            Show_Str(0, 11, RED, WHITE, "data recording", 16, 0);
        }
    }
    if (sensor_err_count >= SENSOR_ERR_NUM ) {
        sensor_err_count = SENSOR_ERR_NUM;//保证传感器坏时变量不溢出
    }
}
void Display_Main_Param(void )
{
#define DISPLAY_LCDSIZE         24
#define TREND_REFRESH_COUNT     5
    u8* dispaly_buf;
    TagRuntime* pruntime;
    pruntime = get_runtime_value();
    static u8 olddetectstate = 0;
    static u8 trend_refresh_count = TREND_REFRESH_COUNT;
    u16 temp_disp_line, humi_disp_line, dew_disp_line, ppmv_disp_line, unit_offset;
    static enum {
        DISPLAY_PARAM1,
        DISPLAY_PARAM2,
        DISPLAY_PARAM3,
        DISPLAY_PARAM4,
        DISPLAY_PARAM5,
        DISPLAY_PARAM6,
    } d_state = DISPLAY_PARAM1;
    param.display_param.l = 4;
    param.dot_num.l = 1;
    if (2 == param.display_param.l) {
        temp_disp_line = 64;
        humi_disp_line = 64 + 64 * 1;
    } else if (3 == param.display_param.l) {
        temp_disp_line = 64;
        humi_disp_line = 64 + 64 * 1;
        dew_disp_line = 64 + 64 * 2;
    } else {
        temp_disp_line = 64;
        humi_disp_line = 64 + 64 * 1;
        dew_disp_line = 64 + 64 * 2;
        ppmv_disp_line = 64 + 64 * 3;
    }
    unit_offset = 16;
    switch (pruntime->lenv_value) {
        case 2:
            if (param.fastdew_enable.l == 0) {
                switch (d_state) {
                    case DISPLAY_PARAM1:
                        dprintf_om(pruntime->drew_value, (u8)param.dot_num.l, 0, 2, 0, LCDsize40, showright);
                        Show_Str(160, 3, BLUE, WHITE, "℃td", LCDsize32, 0);
                        d_state = DISPLAY_PARAM2;
                        break;
                    case DISPLAY_PARAM2:
                        dprintf_om(pruntime->ppmv_value, (u8)param.dot_num.l, 0, 7, 0, LCDsize40, showright);
//                     Gui_Lcd_Unit(ModifyMenuData, 32);
                        Show_Str(160, 8, BLUE, WHITE, "PPMV", LCDsize32, 0);
                        d_state = DISPLAY_PARAM1;
                        break;
                    default:
                        d_state = DISPLAY_PARAM1;
                        break;
                }
                set_detectdew_result(0);
            } else {
                if (olddetectstate != get_detectdew_result()) {
                    d_state = DISPLAY_PARAM1;
                }
                if (get_detectdew_result() == 0) {
                    switch (d_state) {
                        case DISPLAY_PARAM1:
                            Gui_Lcd_Interface();
                            d_state = DISPLAY_PARAM2;
                            break;
                        case DISPLAY_PARAM2:
                            dispaly_buf = lang ? (u8 *)("正在预测露点") : (u8 *)("Detecting dew");
                            Show_Str(16, 2, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                            d_state = DISPLAY_PARAM3;
                            break;
                        case DISPLAY_PARAM3:
                            dprintf_om(pruntime->drew_value, (u8)param.dot_num.l, 58, 6, 0, LCDsize24, showright);
                            Show_Str(156, 6, BLUE, WHITE, "℃td", LCDsize24, 0);
                            d_state = DISPLAY_PARAM4;
                            break;
                        case DISPLAY_PARAM4:
                            dprintf_om(pruntime->ppmv_value, (u8)param.dot_num.l, 58, 9, 0, LCDsize24, showright);
                            Show_Str(156, 9, BLUE, WHITE, "PPMV", LCDsize24, 0);
                            d_state = DISPLAY_PARAM2;
                            break;
                    }
                } else {
                    switch (d_state) {
                        case DISPLAY_PARAM1:
                            Gui_Lcd_Interface();
                            d_state = DISPLAY_PARAM2;
                            break;
                        case DISPLAY_PARAM2:
                            dprintf_om(pruntime->drew_value, (u8)param.dot_num.l, 0, 2, 0, LCDsize40, showright);
                            Show_Str(160, 3, BLUE, WHITE, "℃td", LCDsize32, 0);
                            d_state = DISPLAY_PARAM3;
                            break;
                        case DISPLAY_PARAM3:
                            dprintf_om(pruntime->ppmv_value, (u8)param.dot_num.l, 0, 7, 0, LCDsize40, showright);
                            Show_Str(160, 8, BLUE, WHITE, "PPMV", LCDsize32, 0);
                            d_state = DISPLAY_PARAM4;
                            break;
                        case DISPLAY_PARAM4:
                            dispaly_buf = lang ? (u8 *)("预测完成") : (u8 *)("DetectOK");
                            Show_Str(0, 12, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                            d_state = DISPLAY_PARAM2;
                            break;
                    }
                }
                olddetectstate = get_detectdew_result();
            }
            break;
        case 4:
            switch (pruntime->display_index) {
                case 0:
#if DISPLAY_LCDSIZE == 32
                    switch (d_state) {
                        case DISPLAY_PARAM1:
                            dispaly_buf = lang ? (u8 *)("温度") : (u8 *)("TEMP");
                            Show_Str(0, 0, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                            if (sensor_err_count < SENSOR_ERR_NUM ) {
                                dprintf_om(pruntime->temp_value, (u8)param.calib_ch.c[dot_num],  64, 0, 0, LCDsize32, showright);
                            } else {
                                Show_Str(64, 0, BLUE, WHITE, "    --.--", LCDsize32, 0);
                            }
                            Show_Str(64 + 16 * 8, 0, BLUE, WHITE, "~  ", LCDsize32, 0);//~代替℃
                            d_state = DISPLAY_PARAM2;
                            break;
                        case DISPLAY_PARAM2:
                            dispaly_buf = lang ? (u8 *)("湿度") : (u8 *)("HUMI");
                            Show_Str( 0, 4, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                            if (sensor_err_count < SENSOR_ERR_NUM ) {
                                dprintf_om(pruntime->humi_value, (u8)param.calib_ch.c[dot_num], 64, 4, 0, LCDsize32, showright);
                            } else {
                                Show_Str(64, 4, BLUE, WHITE, "    --.--", LCDsize32, 0);
                            }
                            Show_Str( 64 + 16 * 8, 4, BLUE, WHITE, "%RH", LCDsize32, 0);
                            d_state = DISPLAY_PARAM3;
                            break;
                        case DISPLAY_PARAM3:
                            dispaly_buf = lang ? (u8 *)("露点") : (u8 *)("DEW ");
                            Show_Str( 0, 8, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                            if (sensor_err_count < SENSOR_ERR_NUM ) {
                                dprintf_om(pruntime->drew_value, (u8)param.calib_ch.c[dot_num], 64, 8, 0, LCDsize32, showright);
                            } else {
                                Show_Str(64, 8, BLUE, WHITE, "    --.--", LCDsize32, 0);
                            }
                            Show_Str( 64 + 16 * 8, 8, BLUE, WHITE, "~td", LCDsize32, 0);
                            d_state = DISPLAY_PARAM4;
                            break;
                        case DISPLAY_PARAM4:
                            dispaly_buf = lang ? (u8 *)("微水") : (u8 *)("PPMV");
                            Show_Str(0, 12, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                            if (sensor_err_count < SENSOR_ERR_NUM ) {
                                dprintf_om(pruntime->ppmv_value, 1, 64, 12, 0, LCDsize32, showright);
                            } else {
                                Show_Str(64, 12, BLUE, WHITE, "    --.--", LCDsize32, 0);
                            }
                            d_state = DISPLAY_PARAM1;
                            break;
                        default:
                            break;
                    }
#else
                    switch (d_state) {
                        case DISPLAY_PARAM1:
                            if (trend_refresh_count > 0) {
                                trend_refresh_count--;
                            } else {
                                trend_refresh_count = TREND_REFRESH_COUNT;
//                                GUI_LCD_Fill(0, temp_disp_line, 10, temp_disp_line + 3, BLACK);
                            }
                            dispaly_buf = param.unit_name.l ? (u8 *)("  ~F") : (u8 *)("  ~C");
//                            Show_Str(0, 0, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize16, 0);
//                            Display_Data_Trend(0, temp_disp_line + 1, LCDsize16, get_temp_data_trend(), 0);
                            /**优化了小数点，所以当显示HHH或者0位小数点的数切换到有数据时末尾有残留**/
                            if (param.dot_num.l != 0) {
//                                GUI_LCD_Fill(6 + 7 * 12, temp_disp_line, 10 + 7 * 12, temp_disp_line + 3, BLACK);
                            }
                            if (sensor_err_count < SENSOR_ERR_NUM ) {
                                dprintf_om(pruntime->temp_value, (u8)param.dot_num.l,  32, temp_disp_line, 0, LCDsize32, showright);
                            } else {
                                Show_Str(32, temp_disp_line, BLUE, WHITE, "  --.--", LCDsize32, 0);
                            }
                            Show_Str(144, temp_disp_line + unit_offset, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0); //~代替℃
                            d_state = DISPLAY_PARAM2;
                            break;
                        case DISPLAY_PARAM2:
//                            dispaly_buf = lang ? (u8 *)("湿度") : (u8 *)("HUMI");
//                            Show_Str( 0, 3, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize24, 0);
                            if (trend_refresh_count == 0) {
//                                GUI_LCD_Fill(0, humi_disp_line, 10, humi_disp_line + 3, BLACK);
                            }
//                            Display_Data_Trend(0, humi_disp_line + 1, LCDsize16, get_humi_data_trend(), 0);
                            if (param.dot_num.l != 0) {
//                                GUI_LCD_Fill(6 + 7 * 12, humi_disp_line, 10 + 7 * 12, humi_disp_line + 3, BLACK);
                            }
                            if (sensor_err_count < SENSOR_ERR_NUM ) {
                                dprintf_om(pruntime->humi_value, (u8)param.dot_num.l, 32, humi_disp_line, 0, LCDsize32, showright);
                            } else {
                                Show_Str(32, humi_disp_line, BLUE, WHITE, "  --.--", LCDsize32, 0);
                            }
                            Show_Str( 144, humi_disp_line + unit_offset, BLUE, WHITE, " %RH", LCDsize32, 0);
                            if (2 == param.display_param.l) {
                                d_state = DISPLAY_PARAM5;
                            } else {
                                d_state = DISPLAY_PARAM3;
                            }
                            break;
                        case DISPLAY_PARAM3:
                            if (2 == param.display_param.l) {
                                d_state = DISPLAY_PARAM5;
                                break;
                            }
                            if (trend_refresh_count == 0) {
//                                GUI_LCD_Fill(0, dew_disp_line, 10, dew_disp_line + 3, BLACK);
                            }
                            dispaly_buf = param.unit_name.l ? (u8 *)("Dp~F") : (u8 *)("Dp~C");
//                            Show_Str( 0, 6, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize24, 0);
//                            Display_Data_Trend(0, dew_disp_line + 1, LCDsize16, get_dewpoint_data_trend(), 0);
                            if (param.dot_num.l != 0) {
//                                GUI_LCD_Fill(6 + 7 * 12, dew_disp_line, 10 + 7 * 12, dew_disp_line + 3, BLACK);
                            }
                            if (sensor_err_count < SENSOR_ERR_NUM ) {
                                dprintf_om(pruntime->drew_value, (u8)param.dot_num.l, 32, dew_disp_line, 0, LCDsize32, showright);
                            } else {
                                Show_Str(32, dew_disp_line, BLUE, WHITE, "  --.--", LCDsize32, 0);
                            }
                            Show_Str( 144, dew_disp_line + unit_offset, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                            if (3 == param.display_param.l) {
                                d_state = DISPLAY_PARAM5;
                            } else {
                                d_state = DISPLAY_PARAM4;
                            }
                            break;
                        case DISPLAY_PARAM4:
                            if (4 != param.display_param.l) {
                                d_state = DISPLAY_PARAM5;
                                break;
                            }
//                            dispaly_buf = lang ? (u8 *)("微水") : (u8 *)("PPMV");
//                            Show_Str(0, 9, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                            if (trend_refresh_count == 0) {
//                                GUI_LCD_Fill(0, 9, 10, 12, BLACK);
                            }
                            if (sensor_err_count < SENSOR_ERR_NUM ) {
                                if ((u8)param.dot_num.l >= 1) {
                                    if (pruntime->ppmv_value > 999999) {
                                        dprintf_om(pruntime->ppmv_value / 10, 0, 32, ppmv_disp_line, 0, LCDsize32, showright);
                                    } else {
                                        if (param.dot_num.l != 0) {
//                                            GUI_LCD_Fill(6 + 7 * 12, 9, 10 + 7 * 12, 9 + 3, BLACK);
                                        }
                                        dprintf_om(pruntime->ppmv_value, 1, 32, ppmv_disp_line, 0, LCDsize32, showright);
                                    }
                                } else {
                                    dprintf_om(pruntime->ppmv_value / 10, 0, 32, ppmv_disp_line, 0, LCDsize32, showright);
                                }
                            } else {
                                Show_Str(32, ppmv_disp_line, BLUE, WHITE, "  --.--", LCDsize32, 0);
                            }
                            Show_Str( 144, ppmv_disp_line + unit_offset, BLUE, WHITE, " PPM", LCDsize32, 0);
//                            Display_Data_Trend(0, 10, LCDsize16, get_ppmv_data_trend(), 0);
                            d_state = DISPLAY_PARAM5;
                            break;
                        case DISPLAY_PARAM5:
//                            Show_Str( 0, 12, BLUE, WHITE, "Dew point instrument", LCDsize24, 0);
                            if (sensor_err_count < SENSOR_ERR_NUM ) {

                            } else {
//                                init_alarm_ch_param(0);
//                                init_alarm_ch_param(1);
//                                deinit_probe_data_trend();
                                SensorOnlineFlag = 0;
                            }
//                            Display_Alarm_Icon();
                            d_state = DISPLAY_PARAM1;
                            break;
                        default:
                            break;
                    }
#endif
                    break;
                case 1:
                    dispaly_buf = param.unit_name.l ? (u8 *)(" ℉ ") : (u8 *)(" ℃ ");
//                    Show_Str(88, 2, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    if (sensor_err_count < SENSOR_ERR_NUM ) {
                        if ((u8)param.dot_num.l != 0) {
                            GUI_LCD_Fill(114, 1, 126, 1 + 6, BLACK);
                            if (((pruntime->temp_value < -999) || (pruntime->temp_value > 9999)) && (param.dot_num.l == 2)) {
                                /**大字显示，温度小于-9.99°时，要自动减少一位小数点显示,不做四舍五入**/
                                dprintf_om(pruntime->temp_value / 10, (u8)param.dot_num.l - 1,  6, 1, 2, LCDsize48, showright);
                            } else {
                                dprintf_om(pruntime->temp_value, (u8)param.dot_num.l, 6, 1, 2, LCDsize48, showright);
                            }
                        } else {
                            dprintf_om(pruntime->temp_value, (u8)param.dot_num.l, 6, 1, 2, LCDsize48, showright);
                        }
                    } else {
                        GUI_LCD_Fill(114, 1, 126, 1 + 6, BLACK);
                        Show_Str(6, 1, BLUE, WHITE, ";;<;;", LCDsize48, 0);//数组不足，"--.--"用";;<;;"代替
                    }
                    Show_Str(64, 7, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    break;
                case 3:
//                    dispaly_buf = lang ? (u8 *)("湿度") : (u8 *)("HUMI");
//                    Show_Str( 88, 2, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    if (sensor_err_count < SENSOR_ERR_NUM ) {
                        if ((u8)param.dot_num.l != 0) {
                            GUI_LCD_Fill(114, 1, 126, 1 + 6, BLACK);
                            dprintf_om(pruntime->humi_value, (u8)param.dot_num.l, 6, 1, 2, LCDsize48, showright);
                        } else {
                            dprintf_om(pruntime->humi_value, (u8)param.dot_num.l, 6, 1, 2, LCDsize48, showright);
                        }
                    } else {
                        GUI_LCD_Fill(114, 1, 126, 1 + 6, BLACK);
                        Show_Str(6, 1, BLUE, WHITE, ";;<;;", LCDsize48, 0);
                    }
                    Show_Str( 64, 7, BLUE, WHITE, "%RH ", LCDsize32, 0);
                    break;
                case 5:
                    dispaly_buf = param.unit_name.l ? (u8 *)("DP℉") : (u8 *)("DP℃");
//                    Show_Str( 88, 2, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    if (sensor_err_count < SENSOR_ERR_NUM ) {
                        if ((u8)param.dot_num.l != 0) {
                            GUI_LCD_Fill(114, 1, 126, 1 + 6, BLACK);
                            if (((pruntime->drew_value < -999) || (pruntime->drew_value > 9999)) && (param.dot_num.l == 2)) {
                                dprintf_om(pruntime->drew_value / 10, (u8)param.dot_num.l - 1, 6, 1, 2, LCDsize48, showright);
                            } else {
                                dprintf_om(pruntime->drew_value, (u8)param.dot_num.l, 6, 1, 2, LCDsize48, showright);
                            }
                        } else {
                            dprintf_om(pruntime->drew_value, (u8)param.dot_num.l, 6, 1, 2, LCDsize48, showright);

                        }
                    } else {
                        GUI_LCD_Fill(114, 1, 126, 1 + 6, BLACK);
                        Show_Str(6, 1, BLUE, WHITE, ";;<;;", LCDsize48, 0);
                    }
                    Show_Str( 64, 7, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    break;
                case 7:
//                    dispaly_buf = lang ? (u8 *)("微水") : (u8 *)("PPMV");
//                    Show_Str(88, 2, BLUE, WHITE, (u8 *)dispaly_buf, LCDsize32, 0);
                    if (sensor_err_count < SENSOR_ERR_NUM ) {
                        if (pruntime->ppmv_value > 999999) {
                            dprintf_om(pruntime->ppmv_value / 10, 0, 6, 2, 1, LCDsize40, showright);
                        } else {
                            dprintf_om(pruntime->ppmv_value / 10, 0, 6, 1, 2, LCDsize48, showright);
                        }
                    } else {
                        GUI_LCD_Fill(114, 1, 126, 1 + 6, BLACK);
                        Show_Str(6, 1, BLUE, WHITE, ";;<;;", LCDsize48, 0);
                    }
                    Show_Str( 64, 7, BLUE, WHITE, "PPM ", LCDsize32, 0);
                    break;
                case 2:
                case 4:
                case 6:
                case 8:
                    break;
                default:
                    break;
            }
        default:
            break;
    }
    if (TimeInterval != 0) {
        if ((4 == pruntime->lenv_value) && (pruntime->display_index > 0)) {
            Show_Str(0, 11, RED, WHITE, "data recording", 16, 0);
        }
    }
    if (sensor_err_count >= SENSOR_ERR_NUM ) {
        sensor_err_count = SENSOR_ERR_NUM;//保证传感器坏时变量不溢出
    }
}
void Display_Data_Trend(u16 x, u16 y, u8 size, trend_state datatrend, u8 mode)
{
    u16 i;
    trend_state trend_state = GENTLY_TREND;
    const  u8 gently_buf[5] = {0x14, 0x36, 0x77, 0x36, 0x14};
    const  u8 uptrend_buf[5] = {0x04, 0x06, 0x07, 0x06, 0x04};
    const  u8 downtrend_buf[5] = {0x10, 0x30, 0x70, 0x30, 0x10};
    u8 display_buf[5];

    trend_state = datatrend;
    switch (trend_state) {
        case NO_DATA_TREND:
            for (i = 0; i < 5; i++) {

                display_buf[i] = 0xfe;
            }
            break;
        case GENTLY_TREND:
            for (i = 0; i < 5; i++) {
                display_buf[i] = gently_buf[i];
            }
            break;
        case UP_TREND:
            for (i = 0; i < 5; i++) {
                display_buf[i] = uptrend_buf[i];
            }
            break;
        case DOWN_TREND:
            for (i = 0; i < 5; i++) {
                display_buf[i] = downtrend_buf[i];
            }
            break;
        default:
            break;
    }

    pdev->LCD_WR_REG(0Xb4 + y);
    for (i = 0; i < 5; i++) {
        pdev->LCD_WR_REG(0x10 + (x + i) / 16);
        pdev->LCD_WR_REG(0x00 + (x + i) % 16);
        if (!mode ) {
            pdev->LCD_WR_DATA(display_buf[i]);
            pdev->LCD_WR_DATA(display_buf[i]);
        } else {
            pdev->LCD_WR_DATA(~display_buf[i]);
            pdev->LCD_WR_DATA(~display_buf[i]);
        }
    }
}
//extern fsm_rt_t skip_time_Nx100ms(uint time_100ms);
//extern u8 get_ch_alarm1_icon(void);
//extern u8 get_ch_alarm2_icon(void);
//void Display_Alarm_Icon(void)
//{
//    static enum {
//        ICON_START,
//        ICON_DISPLAY,
//        ICON_DELAY1,
//        ICON_CLEAR,
//        ICON_DELAY2,
//    } s_icon = ICON_START;
//    static u8 alarm1_icon_state = 0, alarm2_icon_state = 0;
//    u8 temp_disp_line, humi_disp_line, dew_disp_line;

//    if (2 == param.display_param.l) {
//        temp_disp_line = 2;
//        humi_disp_line = 7;
//    } else if (3 == param.display_param.l) {
//        temp_disp_line = 0;
//        humi_disp_line = 4;
//        dew_disp_line = 8;
//    } else {
//        temp_disp_line = 0;
//        humi_disp_line = 3;
//        dew_disp_line = 6;
//    }
//    switch (s_icon) {
//        case ICON_START:
//            alarm1_icon_state = get_ch_alarm1_icon();
//            alarm2_icon_state = get_ch_alarm2_icon();
//            if (( alarm1_icon_state != 0x00) || (alarm2_icon_state != 0x00)) {
//                s_icon = ICON_DISPLAY;
//            }
//            break;
//        case ICON_DISPLAY:
//            if ((alarm1_icon_state == 0x01) || (alarm2_icon_state == 0x01)) {
//                Show_Str( 116, temp_disp_line, BLUE, WHITE, "^", LCDsize16, 0);
//            }
//            if ((alarm1_icon_state == 0x02) || (alarm2_icon_state == 0x02)) {
//                Show_Str( 116, humi_disp_line, BLUE, WHITE, "^", LCDsize16, 0);
//            }
//            if (((alarm1_icon_state == 0x04) || (alarm2_icon_state == 0x04)) && (param.display_param.l >= 3)) {
//                Show_Str( 116, dew_disp_line, BLUE, WHITE, "^", LCDsize16, 0);
//            }
//            if (((alarm1_icon_state == 0x08) || (alarm2_icon_state == 0x08)) && (param.display_param.l >= 4)) {
//                Show_Str( 116, 9, BLUE, WHITE, "^", LCDsize16, 0);
//            }
//            s_icon = ICON_DELAY1;
//            break;
//        case ICON_DELAY1:
//            if (fsm_rt_cpl == skip_time_Nx100ms(6)) {
//                s_icon = ICON_CLEAR;
//            }
//            break;
//        case ICON_CLEAR:
//            if ((alarm1_icon_state == 0x01) || (alarm2_icon_state == 0x01)) {
//                Show_Str( 116, temp_disp_line, BLUE, WHITE, " ", LCDsize16, 0);
//            }
//            if ((alarm1_icon_state == 0x02) || (alarm2_icon_state == 0x02)) {
//                Show_Str( 116, humi_disp_line, BLUE, WHITE, " ", LCDsize16, 0);
//            }
//            if (((alarm1_icon_state == 0x04) || (alarm2_icon_state == 0x04)) && (param.display_param.l >= 3)) {
//                Show_Str( 116, dew_disp_line, BLUE, WHITE, " ", LCDsize16, 0);
//            }
//            if (((alarm1_icon_state == 0x08) || (alarm2_icon_state == 0x08)) && (param.display_param.l >= 4)) {
//                Show_Str( 116, 9, BLUE, WHITE, " ", LCDsize16, 0);
//            }
//            s_icon = ICON_DELAY2;
//            break;
//        case ICON_DELAY2:
//            if (fsm_rt_cpl == skip_time_Nx100ms(4)) {
//                s_icon = ICON_START;
//            }
//            break;
//        default:
//            alarm1_icon_state = 0;
//            alarm2_icon_state = 0;
//            break;
//    }
//}
//void set_display_switch_time(u16 s_displaytime)
//{
//    display_swtich_time = s_displaytime;
//}
//void Auto_Switch_Display(void)
//{
//#define AUTO_SWITCH_TIME  20   //500ms一次
//    TagRuntime* pruntime;
//    if (param.display_mode.l != 0x01) {
//        display_swtich_time = 0;
//        return;
//    }
//    pruntime = get_runtime_value();
//    if ((pruntime->lenv_value != 4) || (WorkState != WorkNormal)) {
//        display_swtich_time = 0;
//        return;
//    }

//    display_swtich_time++;
//    if (display_swtich_time >= AUTO_SWITCH_TIME) {
//        display_swtich_time = 0;
//        Gui_Lcd_Clear(BLACK);
//        dispaly_pwr_state = 1;
//        pruntime->display_index++;
//        if (pruntime->display_index > param.display_param.l * 2) {
//            pruntime->display_index = 0;
//        }
//        if ((pruntime->display_index % 2 == 0) && (pruntime->display_index != 0)) {
//            if (target_is_watch_meter()) {
//                pruntime->display_index++;
//                if (pruntime->display_index == 5) {
//                    /*dp_watch only have big character display*/
//                    pruntime->display_index = pruntime->display_index + 2;
//                }
//                if (pruntime->display_index > param.display_param.l * 2) {
//                    pruntime->display_index = 0;
//                }
//                Curve.gui_paint_mode = 0;
//            } else {
//                Curve.gui_paint_mode = 1;
//            }
//        } else {
//            Curve.gui_paint_mode = 0;
//        }
////        RTCFlag = UPDATE_REPAT_5;
//    }
//}

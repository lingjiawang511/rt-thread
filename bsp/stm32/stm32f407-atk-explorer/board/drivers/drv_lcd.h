//////////////////////////////////////////////////////////////////////////////////
#ifndef __DRV_LCD_H
#define __DRV_LCD_H
#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include "rtthread.h"

/* type 0:DATA 1:REG 2: DELAY */
typedef enum {
    LCD_WR_DAT_TYPE,
    LCD_WR_REG_TYPE,
    LCD_WR_DLY_TYPE,
    LCD_WR_MAX_TYPE
} LCD_OP_TYPE;

typedef enum {
    LCD_POWER_OFF,
    LCD_POWER_ON,
} LCD_Power_Type;
//LCD地址结构体
typedef struct {
    __IO rt_uint16_t LCD_REG;
    __IO rt_uint16_t LCD_RAM;
} LCD_TypeDef;
//使用NOR/SRAM的 Bank1.sector1,地址位HADDR[27,26]=00 A16作为数据命令区分线
//注意设置时STM32内部会右移一位对其! 1 1111 1111 1 111 1110=0x1FFFE
#if defined(SOC_STM32F407ZG)
#define LCD_BASE         ((rt_uint32_t)(0x6C000000 | 0x0000007E))
#else
#define LCD_BASE         ((rt_uint32_t)(0x60000000 | 0x0001FFFE))
#endif
#define LCD             ((LCD_TypeDef *) LCD_BASE)
//////////////////////////////////////////////////////////////////////////////////

//扫描方向定义
#define L2R_U2D  0      //从左到右,从上到下
#define L2R_D2U  1      //从左到右,从下到上
#define R2L_U2D  2      //从右到左,从上到下
#define R2L_D2U  3      //从右到左,从下到上

#define U2D_L2R  4      //从上到下,从左到右
#define U2D_R2L  5      //从上到下,从右到左
#define D2U_L2R  6      //从下到上,从左到右
#define D2U_R2L  7      //从下到上,从右到左

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向

//画笔颜色
#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40 //棕色
#define BRRED            0XFC07 //棕红色
#define GRAY             0X8430 //灰色
//GUI颜色

#define DARKBLUE         0X01CF //深蓝色
#define LIGHTBLUE        0X7D7C //浅蓝色
#define GRAYBLUE         0X5458 //灰蓝色
//以上三色为PANEL的颜色

#define LIGHTGREEN       0X841F //浅绿色
//#define LIGHTGRAY        0XEF5B //浅灰色(PANNEL)
#define LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)

//LCD分辨率设置
#define SSD_HOR_RESOLUTION      800     //LCD水平分辨率
#define SSD_VER_RESOLUTION      480     //LCD垂直分辨率
//LCD驱动参数设置
#define SSD_HOR_PULSE_WIDTH     1       //水平脉宽
#define SSD_HOR_BACK_PORCH      46      //水平前廊
#define SSD_HOR_FRONT_PORCH     210     //水平后廊

#define SSD_VER_PULSE_WIDTH     1       //垂直脉宽
#define SSD_VER_BACK_PORCH      23      //垂直前廊
#define SSD_VER_FRONT_PORCH     22      //垂直前廊
//如下几个参数，自动计算
#define SSD_HT  (SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_BACK_PORCH)
#define SSD_VT  (SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

typedef struct {
    rt_uint16_t width;          //LCD 宽度
    rt_uint16_t height;         //LCD 高度
    rt_uint16_t id;             //LCD ID
    rt_uint8_t  dir;            //横屏还是竖屏控制：0，竖屏；1，横屏。
    rt_uint16_t wramcmd;        //开始写gram指令
    rt_uint16_t setxcmd;        //设置x坐标指令
    rt_uint16_t setycmd;        //设置y坐标指令

    rt_uint16_t  (*LCD_RD_DATA)(void);
    void (*LCD_WR_DATA)(__IO rt_uint16_t data);
    void (*LCD_WR_REG)(__IO rt_uint16_t Reg);
    void (*LCD_WriteReg)(rt_uint16_t LCD_Reg, rt_uint16_t LCD_RegValue);
    rt_uint16_t  (*LCD_ReadReg)(rt_uint16_t LCD_Reg);
    rt_err_t (*LCD_Init)(void);
    void (*LCD_DisplayOn)(void);
    void (*LCD_DisplayOff)(void);
    void (*LCD_Clear)(rt_uint16_t Color);
    void (*LCD_Fill)(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint16_t color);
    void (*LCD_Color_Fill)(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint16_t *color);
    void (*LCD_SetCursor)(rt_uint16_t Xpos, rt_uint16_t Ypos);
    void (*LCD_Set_Window)(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t width, rt_uint16_t height);
    void (*LCD_DrawPoint)(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color);
    void (*LCD_Fast_DrawPoint)(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color);
    rt_uint16_t  (*LCD_ReadPoint)(rt_uint16_t x, rt_uint16_t y);
    void (*LCD_WriteRAM_Prepare)(void);
    void (*LCD_WriteRAM)(rt_uint16_t RGB_Code);
    rt_uint16_t  (*LCD_ReadRAM)(void);
    void (*LCD_Rst)(void);
    void (*LCD_Power_Control)(LCD_Power_Type eState);
    void (*LcdWriteDataMultiple)(rt_uint16_t * pData, int NumItems);
    void (*LcdReadDataMultiple)(rt_uint16_t * pData, int NumItems);
} _lcd_dev;

void register_lcd_dev ( _lcd_dev *p );
#endif






#include "drv_lcd.h"
#include "stdlib.h"
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "drv_lcd_lcdid_0x1408.h"
#include "drv_lcd_lcdid_0x9341.h"
#include "drv_lcd_lcdid_0x5310.h"
#include "drv_lcd_lcdid_0x1963.h"
#include "drv_lcd_lcdid_0x5510.h"


//////////////////////////////////////////////////////////////////////////////////
//-----------------MCU屏 LCD端口定义----------------
#if defined(SOC_STM32F407ZG)
static uint32_t lcd_power = 21;
static uint32_t lcd_backlight = GET_PIN(B, 15);
static uint32_t lcd_rst = 22;
#else
static uint32_t lcd_power = 34;
static uint32_t lcd_backlight = 66;
static uint32_t lcd_rst = 59;
#endif
static SRAM_HandleTypeDef TFTSRAM_Handler;    //SRAM句柄(用于控制LCD)

static void LCD_Clear(rt_uint16_t color);
static void LCD_SetCursor(rt_uint16_t Xpos, rt_uint16_t Ypos);

static void delay_us(rt_uint32_t us)
{
    __IO uint32_t Delay = us * (SystemCoreClock / 8U / 1000000U);
    do {
        __NOP();
    } while (Delay --);
}
//管理LCD重要参数
//默认为竖屏
static _lcd_dev lcddev;

//写寄存器函数
//regval:寄存器值
static void LCD_WR_REG(__IO rt_uint16_t regval)
{
    regval = regval;    //使用-O2优化的时候,必须插入的延时
    LCD->LCD_REG = regval; //写入要写的寄存器序号
}
//写LCD数据
//data:要写入的值
static void LCD_WR_DATA(__IO rt_uint16_t data)
{
    data = data;        //使用-O2优化的时候,必须插入的延时
    LCD->LCD_RAM = data;
}

//读LCD数据
//返回值:读到的值
static  rt_uint16_t LCD_RD_DATA(void)
{
    __IO rt_uint16_t ram;           //防止被优化
    ram = LCD->LCD_RAM;
    return ram;
}

static void LcdWriteDataMultiple(rt_uint16_t * pData, int NumItems)
{
    while (NumItems--) {
        // ... TBD by user
        *pData = *pData;        //使用-O2优化的时候,必须插入的延时
        LCD->LCD_RAM = *pData++;
    }
}

static void LcdReadDataMultiple(rt_uint16_t * pData, int NumItems)
{
    while (NumItems--) {
        // ... TBD by user
    }
}

//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
static void LCD_WriteReg(rt_uint16_t LCD_Reg, rt_uint16_t LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;     //写入要写的寄存器序号
    LCD->LCD_RAM = LCD_RegValue;//写入数据
}
//读寄存器
//LCD_Reg:寄存器地址
//返回值:读到的数据
static rt_uint16_t LCD_ReadReg(rt_uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);        //写入要读的寄存器序号
    delay_us(5);
    return LCD_RD_DATA();       //返回读到的值
}
//开始写GRAM
static void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}
//LCD写GRAM
//RGB_Code:颜色值
static void LCD_WriteRAM(rt_uint16_t RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;//写十六位GRAM
}

//当mdk -O1时间优化时需要设置
//延时i
static void opt_delay(rt_uint8_t i)
{
    while (i--);
}
//读取个某点的颜色值
//x,y:坐标
//返回值:此点的颜色
static rt_uint16_t LCD_ReadPoint(rt_uint16_t x, rt_uint16_t y)
{
    rt_uint16_t r = 0, g = 0, b = 0;
    if (x >= lcddev.width || y >= lcddev.height)return 0; //超过了范围,直接返回
    LCD_SetCursor(x, y);
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X1963 || lcddev.id == 0X1408)LCD_WR_REG(0X2E); //9341/3510/1963 发送读GRAM指令
    else if (lcddev.id == 0X5510)LCD_WR_REG(0X2E00); //5510 发送读GRAM指令
    r = LCD_RD_DATA();                              //dummy Read
    if (lcddev.id == 0X1963) {
        return r;                   //1963,1408直接读就可以
    } else if (lcddev.id == 0X1408) {
        /**In all color modes, the Frame memory read is always 24 bits so there is no restriction on the length of parameters.
        If data is not transferred in units of pixels, the extra data is regarded as invalid. When this command is read via DSI
        or MDDI, dummy read operation is not performed.目前读取数据有误，不成功**/
        r = LCD_RD_DATA();
        return r;
    }
    opt_delay(2);
    r = LCD_RD_DATA();                              //实际坐标颜色
    //9341/NT35310/NT35510要分2次读出
    opt_delay(2);
    b = LCD_RD_DATA();
    g = r & 0XFF;   //对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); //ILI9341/NT35310/NT35510需要公式转换一下
}
//LCD开启显示
static void LCD_DisplayOn(void)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X1963 || lcddev.id == 0X1408)LCD_WR_REG(0X29); //开启显示
    else if (lcddev.id == 0X5510)LCD_WR_REG(0X2900); //开启显示
}
//LCD关闭显示
static void LCD_DisplayOff(void)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X1963 || lcddev.id == 0X1408)LCD_WR_REG(0X28); //关闭显示
    else if (lcddev.id == 0X5510)LCD_WR_REG(0X2800); //关闭显示
}
//设置光标位置(对RGB屏无效)
//Xpos:横坐标
//Ypos:纵坐标
static void LCD_SetCursor(rt_uint16_t Xpos, rt_uint16_t Ypos)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310) {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
    } else if (lcddev.id == 0X1963) {
        if (lcddev.dir == 0) { //x坐标需要变换
            Xpos = lcddev.width - 1 - Xpos;
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(0);
            LCD_WR_DATA(0);
            LCD_WR_DATA(Xpos >> 8);
            LCD_WR_DATA(Xpos & 0XFF);
        } else {
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(Xpos >> 8);
            LCD_WR_DATA(Xpos & 0XFF);
            LCD_WR_DATA((lcddev.width - 1) >> 8);
            LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        }
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);

    } else if (lcddev.id == 0X5510) {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(Ypos & 0XFF);
    } else if (lcddev.id == 0X1408) {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    }
}
//设置LCD的自动扫描方向(对RGB屏无效)
//注意:其他函数可能会受到此函数设置的影响(尤其是9341),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9341/5310/5510/1963等IC已经实际测试
static void LCD_Scan_Dir(rt_uint8_t dir)
{
    rt_uint16_t regval = 0;
    rt_uint16_t dirreg = 0;
    rt_uint16_t temp;
    if ((lcddev.dir == 1 && lcddev.id != 0X1963) || (lcddev.dir == 0 && lcddev.id == 0X1963)) { //横屏时，对1963不改变扫描方向！竖屏时1963改变方向
        switch (dir) { //方向转换
            case 0:
                dir = 6;
                break;
            case 1:
                dir = 7;
                break;
            case 2:
                dir = 4;
                break;
            case 3:
                dir = 5;
                break;
            case 4:
                dir = 1;
                break;
            case 5:
                dir = 0;
                break;
            case 6:
                dir = 3;
                break;
            case 7:
                dir = 2;
                break;
        }
    }
    if (lcddev.id == 0x9341 || lcddev.id == 0X5310 || lcddev.id == 0X5510 || lcddev.id == 0X1963 || lcddev.id == 0X1408) { //9341/5310/5510/1963,特殊处理
        switch (dir) {
            case L2R_U2D://从左到右,从上到下
                regval |= (0 << 7) | (0 << 6) | (0 << 5);
                break;
            case L2R_D2U://从左到右,从下到上
                regval |= (1 << 7) | (0 << 6) | (0 << 5);
                break;
            case R2L_U2D://从右到左,从上到下
                regval |= (0 << 7) | (1 << 6) | (0 << 5);
                break;
            case R2L_D2U://从右到左,从下到上
                regval |= (1 << 7) | (1 << 6) | (0 << 5);
                break;
            case U2D_L2R://从上到下,从左到右
                regval |= (0 << 7) | (0 << 6) | (1 << 5);
                break;
            case U2D_R2L://从上到下,从右到左
                regval |= (0 << 7) | (1 << 6) | (1 << 5);
                break;
            case D2U_L2R://从下到上,从左到右
                regval |= (1 << 7) | (0 << 6) | (1 << 5);
                break;
            case D2U_R2L://从下到上,从右到左
                regval |= (1 << 7) | (1 << 6) | (1 << 5);
                break;
        }
        if (lcddev.id == 0X5510)dirreg = 0X3600;
        else dirreg = 0X36;
        if ((lcddev.id != 0X5310) && (lcddev.id != 0X5510) && (lcddev.id != 0X1963) && (lcddev.id != 0X1408))regval |= 0X08; //5310/5510/1963不需要BGR
        LCD_WriteReg(dirreg, regval);
        if (lcddev.id != 0X1963 && lcddev.id != 0X1408) { //1963不做坐标处理
            if (regval & 0X20) {
                if (lcddev.width < lcddev.height) { //交换X,Y
                    temp = lcddev.width;
                    lcddev.width = lcddev.height;
                    lcddev.height = temp;
                }
            } else {
                if (lcddev.width > lcddev.height) { //交换X,Y
                    temp = lcddev.width;
                    lcddev.width = lcddev.height;
                    lcddev.height = temp;
                }
            }
        }
        if (lcddev.id == 0X5510) {
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setxcmd + 1);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setxcmd + 2);
            LCD_WR_DATA((lcddev.width - 1) >> 8);
            LCD_WR_REG(lcddev.setxcmd + 3);
            LCD_WR_DATA((lcddev.width - 1) & 0XFF);
            LCD_WR_REG(lcddev.setycmd);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setycmd + 1);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setycmd + 2);
            LCD_WR_DATA((lcddev.height - 1) >> 8);
            LCD_WR_REG(lcddev.setycmd + 3);
            LCD_WR_DATA((lcddev.height - 1) & 0XFF);
        } else {
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(0);
            LCD_WR_DATA(0);
            LCD_WR_DATA((lcddev.width - 1) >> 8);
            LCD_WR_DATA((lcddev.width - 1) & 0XFF);
            LCD_WR_REG(lcddev.setycmd);
            LCD_WR_DATA(0);
            LCD_WR_DATA(0);
            LCD_WR_DATA((lcddev.height - 1) >> 8);
            LCD_WR_DATA((lcddev.height - 1) & 0XFF);
        }
    }
}
//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
static void LCD_DrawPoint(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color)
{
    LCD_SetCursor(x, y);    //设置光标位置
    LCD_WriteRAM_Prepare(); //开始写入GRAM
    LCD->LCD_RAM = color;
}
//快速画点
//x,y:坐标
//color:颜色
static void LCD_Fast_DrawPoint(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310) {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    } else if (lcddev.id == 0X5510) {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(y & 0XFF);
    } else if (lcddev.id == 0X1963) {
        if (lcddev.dir == 0)x = lcddev.width - 1 - x;
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    } else if (lcddev.id == 0X1408) {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }
    LCD->LCD_REG = lcddev.wramcmd;
    LCD->LCD_RAM = color;
}
//SSD1963 背光设置
//pwm:背光等级,0~100.越大越亮.
static void LCD_SSD_BackLightSet(rt_uint8_t pwm)
{
    LCD_WR_REG(0xBE);   //配置PWM输出
    LCD_WR_DATA(0x05);  //1设置PWM频率
    LCD_WR_DATA(pwm * 2.55); //2设置PWM占空比
    LCD_WR_DATA(0x01);  //3设置C
    LCD_WR_DATA(0xFF);  //4设置D
    LCD_WR_DATA(0x00);  //5设置E
    LCD_WR_DATA(0x00);  //6设置F
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
static void LCD_Display_Dir(rt_uint8_t dir)
{
    lcddev.dir = dir;   //竖屏/横屏
    if (dir == 0) {     //竖屏
        lcddev.width = 240;
        lcddev.height = 320;
        if (lcddev.id == 0X9341 || lcddev.id == 0X5310) {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
            if (lcddev.id == 0X5310) {
                lcddev.width = 320;
                lcddev.height = 480;
            } else if (lcddev.id == 0X1408) { //R61408默认的扫描方式是竖屏扫描
                lcddev.width = 480;
                lcddev.height = 800;
            }
        } else if (lcddev.id == 0x5510) {
            lcddev.wramcmd = 0X2C00;
            lcddev.setxcmd = 0X2A00;
            lcddev.setycmd = 0X2B00;
            lcddev.width = 480;
            lcddev.height = 800;
        } else if (lcddev.id == 0X1963) {
            lcddev.wramcmd = 0X2C;  //设置写入GRAM的指令
            lcddev.setxcmd = 0X2B;  //设置写X坐标指令
            lcddev.setycmd = 0X2A;  //设置写Y坐标指令
            lcddev.width = 480;     //设置宽度480
            lcddev.height = 800;    //设置高度800
        } else if (lcddev.id == 0X1408) {
            lcddev.wramcmd = 0X2C;  //设置写入GRAM的指令
            lcddev.setxcmd = 0X2A;  //设置写X坐标指令
            lcddev.setycmd = 0X2B;  //设置写Y坐标指令
            lcddev.width = 480;     //设置宽度480
            lcddev.height = 800;    //设置高度800
        }
    } else {             //横屏
        lcddev.width = 320;
        lcddev.height = 240;
        if (lcddev.id == 0X9341 || lcddev.id == 0X5310) {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
        } else if (lcddev.id == 0x5510) {
            lcddev.wramcmd = 0X2C00;
            lcddev.setxcmd = 0X2A00;
            lcddev.setycmd = 0X2B00;
            lcddev.width = 800;
            lcddev.height = 480;
        } else if (lcddev.id == 0X1963 || lcddev.id == 0X1408) {
            lcddev.wramcmd = 0X2C;  //设置写入GRAM的指令
            lcddev.setxcmd = 0X2A;  //设置写X坐标指令
            lcddev.setycmd = 0X2B;  //设置写Y坐标指令
            lcddev.width = 800;     //设置宽度800
            lcddev.height = 480;    //设置高度480
        } else if (lcddev.id == 0X1408) {
            lcddev.wramcmd = 0X2C;  //设置写入GRAM的指令
            lcddev.setxcmd = 0X2A;  //设置写X坐标指令
            lcddev.setycmd = 0X2B;  //设置写Y坐标指令
            lcddev.width = 800;     //设置宽度800
            lcddev.height = 480;    //设置高度480
        }
        if (lcddev.id == 0X5310) {
            lcddev.width = 480;
            lcddev.height = 320;
        }
    }
    LCD_Scan_Dir(DFT_SCAN_DIR); //默认扫描方向
}
//设置窗口(对RGB屏无效),并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
static void LCD_Set_Window(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t width, rt_uint16_t height)
{
    rt_uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X1408 || (lcddev.dir == 1 && lcddev.id == 0X1963)) {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_DATA(theight & 0XFF);
    } else if (lcddev.id == 0X1963) { //1963竖屏特殊处理
        sx = lcddev.width - width - sx;
        height = sy + height - 1;
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_DATA((sx + width - 1) >> 8);
        LCD_WR_DATA((sx + width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_DATA(height >> 8);
        LCD_WR_DATA(height & 0XFF);
    } else if (lcddev.id == 0X5510) {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA(theight & 0XFF);
    }
}
//SRAM底层驱动，时钟使能，引脚分配
//此函数会被HAL_SRAM_Init()调用
//hsram:SRAM句柄
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_FSMC_CLK_ENABLE();            //使能FSMC时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();           //使能GPIOD时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();           //使能GPIOE时钟
    __HAL_RCC_GPIOF_CLK_ENABLE();           //使能GPIOF时钟
#if defined(SOC_STM32F407ZG)
    __HAL_RCC_GPIOG_CLK_ENABLE();           //使能GPIOF时钟
#endif
#if !defined(SOC_STM32F407ZG)
    //初始化PD0,1,4,5,8,9,10,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | \
                       GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
#else
    //初始化PD0,1,4,5,8,9,10,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | \
                       GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_14 | GPIO_PIN_15;

#endif
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;    //推挽复用
    GPIO_Initure.Pull = GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //高速
    GPIO_Initure.Alternate = GPIO_AF12_FSMC; //复用为FSMC
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);    //初始化

    //初始化PE7,8,9,10,11,12,13,14,15
    GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                       GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);
    //初始化PE7,8,9,10,11,12,13,14,15
    GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                       GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);
#if defined(SOC_STM32F407ZG)
    GPIO_Initure.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);
    GPIO_Initure.Pin = GPIO_PIN_12;
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);
#endif
}

static void LCD_Rst(void)
{
    rt_pin_write(lcd_rst, GPIO_PIN_RESET);
    HAL_Delay(1);
    rt_pin_write(lcd_rst, GPIO_PIN_SET);
    HAL_Delay(1);
}
//初始化lcd
//该初始化函数可以初始化各种型号的LCD(详见本.c文件最前面的描述)
static rt_err_t LCD_Init(void)
{
    rt_uint32_t i;
    FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
    FSMC_NORSRAM_TimingTypeDef FSMC_WriteTim;


    TFTSRAM_Handler.Instance = FSMC_NORSRAM_DEVICE;
    TFTSRAM_Handler.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
#if defined(SOC_STM32F407ZG)
    TFTSRAM_Handler.Init.NSBank = FSMC_NORSRAM_BANK4;
#else
    TFTSRAM_Handler.Init.NSBank = FSMC_NORSRAM_BANK1;                   //使用NE1
#endif
    TFTSRAM_Handler.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE; //地址/数据线不复用
    TFTSRAM_Handler.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;            //SRAM
    TFTSRAM_Handler.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16; //16位数据宽度
    TFTSRAM_Handler.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE; //是否使能突发访问,仅对同步突发存储器有效,此处未用到
    TFTSRAM_Handler.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; //等待信号的极性,仅在突发模式访问下有用
    TFTSRAM_Handler.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS; //存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT
    TFTSRAM_Handler.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;  //存储器写使能
    TFTSRAM_Handler.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;         //等待使能位,此处未用到
    TFTSRAM_Handler.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;      //读写使用不同的时序
    TFTSRAM_Handler.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE; //是否使能同步传输模式下的等待信号,此处未用到
    TFTSRAM_Handler.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;         //禁止突发写
    TFTSRAM_Handler.Init.ContinuousClock = FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;

    //FMC读时序控制寄存器
    FSMC_ReadWriteTim.AddressSetupTime = 0x0F + 10;     //地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns
    FSMC_ReadWriteTim.AddressHoldTime = 0;
    FSMC_ReadWriteTim.DataSetupTime = 60 + 40;          //数据保存时间为60个HCLK    =6*60=360ns
    FSMC_ReadWriteTim.AccessMode = FSMC_ACCESS_MODE_A; //模式A
    //FMC写时序控制寄存器
    FSMC_WriteTim.BusTurnAroundDuration = 0;        //总线周转阶段持续时间为0，此变量不赋值的话会莫名其妙的自动修改为4。导致程序运行正常
    FSMC_WriteTim.AddressSetupTime = 9;             //地址建立时间（ADDSET）为9个HCLK =54ns
    FSMC_WriteTim.AddressHoldTime = 0;
    FSMC_WriteTim.DataSetupTime = 8;                //数据保存时间为6ns*9个HCLK=54n
    FSMC_WriteTim.AccessMode = FSMC_ACCESS_MODE_A;  //模式A
    HAL_SRAM_Init(&TFTSRAM_Handler, &FSMC_ReadWriteTim, &FSMC_WriteTim);

    rt_pin_mode  (lcd_power, PIN_MODE_OUTPUT);
    rt_pin_mode  (lcd_backlight, PIN_MODE_OUTPUT);
    rt_pin_mode  (lcd_rst, PIN_MODE_OUTPUT);

    rt_pin_write(lcd_power, GPIO_PIN_RESET);//开LCD电源
    HAL_Delay(1); // delay 5 ms
    LCD_Rst();


    //尝试9341 ID的读取
    LCD_WR_REG(0XD3);
    lcddev.id = LCD_RD_DATA();  //dummy read
    lcddev.id = LCD_RD_DATA();  //读到0X00
    lcddev.id = LCD_RD_DATA();  //读取93
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA(); //读取41
    if (lcddev.id != 0X9341) {  //非9341,尝试看看是不是NT35310
        LCD_WR_REG(0XD4);
        lcddev.id = LCD_RD_DATA(); //dummy read
        lcddev.id = LCD_RD_DATA(); //读回0X01
        lcddev.id = LCD_RD_DATA(); //读回0X53
        lcddev.id <<= 8;
        lcddev.id |= LCD_RD_DATA(); //这里读回0X10
        if (lcddev.id != 0X5310) {  //也不是NT35310,尝试看看是不是NT35510
            LCD_WR_REG(0XDA00);
            lcddev.id = LCD_RD_DATA();      //读回0X00
            LCD_WR_REG(0XDB00);
            lcddev.id = LCD_RD_DATA();      //读回0X80
            lcddev.id <<= 8;
            LCD_WR_REG(0XDC00);
            lcddev.id |= LCD_RD_DATA();     //读回0X00
            if (lcddev.id == 0x8000)lcddev.id = 0x5510; //NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
            if (lcddev.id != 0X5510) {      //也不是NT5510,尝试看看是不是SSD1963
                LCD_WR_REG(0XA1);   //此寄存器在任何模式下都可读取
                lcddev.id = LCD_RD_DATA();
                lcddev.id = LCD_RD_DATA();  //读回0X57
                lcddev.id <<= 8;
                lcddev.id |= LCD_RD_DATA(); //读回0X61
                if (lcddev.id == 0X5761) {
                    lcddev.id = 0X1963; //SSD1963读回的ID是5761H,为方便区分,我们强制设置为1963
                } else { //读不出数据，先强制让芯片等于0x1408
                    lcddev.id = 0X1408;
                }
            }
        }
    }
//  printf(" LCD ID:%x\r\n",lcddev.id); //打印LCD ID
    if (lcddev.id == 0x1408) {
        for (i = 0; i < sizeof(lcd_config_data_0x1408) / sizeof(lcd_config_data_0x1408[0]); i++) {
            if (0 == lcd_config_data_0x1408[i].type) {
                LCD_WR_DATA(lcd_config_data_0x1408[i].val);
            } else if (1 == lcd_config_data_0x1408[i].type) {
                LCD_WR_REG(lcd_config_data_0x1408[i].val);
            } else {
                HAL_Delay(lcd_config_data_0x1408[i].val);
            }
        }
        HAL_Delay(1);
        LCD_WR_REG(0x36);
//      WriteData(0x60);    //我们的屏正视时从右到左（Y），从上到下（X）
        LCD_WR_DATA(0x00);  //我们的屏正视时从左到右，从上到下
    } else if (lcddev.id == 0X9341) { //9341初始化
        for (i = 0; i < sizeof(lcd_config_data_0x9341) / sizeof(lcd_config_data_0x9341[0]); i++) {
            if (0 == lcd_config_data_0x9341[i].type) {
                LCD_WR_DATA(lcd_config_data_0x9341[i].val);
            } else if (1 == lcd_config_data_0x9341[i].type) {
                LCD_WR_REG(lcd_config_data_0x9341[i].val);
            } else {
                HAL_Delay(lcd_config_data_0x9341[i].val);
            }
        }
    } else if (0x5310 == lcddev.id) {
        for (i = 0; i < sizeof(lcd_config_data_0x5310) / sizeof(lcd_config_data_0x5310[0]); i++) {
            if (0 == lcd_config_data_0x5310[i].type) {
                LCD_WR_DATA(lcd_config_data_0x5310[i].val);
            } else if (1 == lcd_config_data_0x5310[i].type) {
                LCD_WR_REG(lcd_config_data_0x5310[i].val);
            } else {
                HAL_Delay(lcd_config_data_0x5310[i].val);
            }
        }
    } else if (lcddev.id == 0x5510) {
        for (i = 0; i < sizeof(lcd_config_data_0x5510) / sizeof(lcd_config_data_0x5510[0]); i++) {
            LCD_WriteReg(lcd_config_data_0x5510[i].reg, lcd_config_data_0x5510[i].val);
        }
        LCD_WR_REG(0x1100);
        delay_us(120);
        LCD_WR_REG(0x2900);
    } else if (lcddev.id == 0X1963) {
        for (i = 0; i < sizeof(lcd_config_data_0x5310) / sizeof(lcd_config_data_0x5310[0]); i++) {
            if (0 == lcd_config_data_0x5310[i].type) {
                LCD_WR_DATA(lcd_config_data_0x5310[i].val);
            } else if (1 == lcd_config_data_0x5310[i].type) {
                LCD_WR_REG(lcd_config_data_0x5310[i].val);
            } else {
                HAL_Delay(lcd_config_data_0x5310[i].val);
            }
        }
        LCD_SSD_BackLightSet(100);//背光设置为最亮
    }

    //初始化完成以后,提速
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X5510 || lcddev.id == 0X1963 || lcddev.id == 0X1408) { //如果是这几个IC,则设置WR时序为最快
        //重新配置写时序控制寄存器的时序
        FSMC_Bank1E->BWTR[6] &= ~(0XF << 0); //地址建立时间(ADDSET)清零
        FSMC_Bank1E->BWTR[6] &= ~(0XF << 8); //数据保存时间清零
        FSMC_Bank1E->BWTR[6] |= 3 << 0; //地址建立时间(ADDSET)为3个HCLK =18ns
        FSMC_Bank1E->BWTR[6] |= 2 << 8; //数据保存时间(DATAST)为6ns*3个HCLK=18ns
    }
    LCD_Display_Dir(0);     //默认为竖屏
    rt_pin_write(lcd_backlight, GPIO_PIN_SET); //点亮背光,后期改为PWM控制
    LCD_Clear(BLUE);
	
    return RT_EOK;
}

//清屏函数
//color:要清屏的填充色
static void LCD_Clear(rt_uint16_t color)
{
    rt_uint32_t index = 0;
    rt_uint32_t totalpoint = lcddev.width;
    totalpoint *= lcddev.height;        //得到总点数
    LCD_SetCursor(0, 0);    //设置光标位置
    LCD_WriteRAM_Prepare();             //开始写入GRAM
    for (index = 0; index < totalpoint; index++) {
        LCD->LCD_RAM = color;
    }
}
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
static void LCD_Fill(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint16_t color)
{
    rt_uint16_t i, j;
    rt_uint16_t xlen = 0;
    xlen = ex - sx + 1;
    for (i = sy; i <= ey; i++) {
        LCD_SetCursor(sx, i);                   //设置光标位置
        LCD_WriteRAM_Prepare();                 //开始写入GRAM
        for (j = 0; j < xlen; j++)LCD->LCD_RAM = color; //显示颜色
    }
}
//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
static void LCD_Color_Fill(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint16_t *color)
{
    rt_uint16_t height, width;
    rt_uint16_t i, j;
    width = ex - sx + 1;    //得到填充的宽度
    height = ey - sy + 1;   //高度
    for (i = 0; i < height; i++) {
        LCD_SetCursor(sx, sy + i);  //设置光标位置
        LCD_WriteRAM_Prepare();     //开始写入GRAM
        for (j = 0; j < width; j++)LCD->LCD_RAM = color[i * width + j]; //写入数据
    }
}

static rt_uint16_t LCD_ReadRAM ( void )
{
    return 0;
}

static void LCD_Power_Control(LCD_Power_Type eState)
{
    if (eState == LCD_POWER_ON) {
        rt_pin_write(lcd_power, GPIO_PIN_RESET);//开LCD电源
    } else {
        rt_pin_write(lcd_power, GPIO_PIN_SET);//关LCD电源
    }
}
void HAL_CRC_MspInit(CRC_HandleTypeDef* hcrc)
{

    if (hcrc->Instance == CRC) {
        __HAL_RCC_CRC_CLK_ENABLE();
    }
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef* hcrc)
{

    if (hcrc->Instance == CRC) {
        __HAL_RCC_CRC_CLK_DISABLE();
    }
}

static rt_err_t general_lcd_init(struct rt_device *dev)
{
    return RT_EOK;
}

static rt_err_t general_lcd_open(struct rt_device *dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t general_lcd_close(struct rt_device *dev)
{
    return RT_EOK;
}

static rt_size_t general_lcd_read(struct rt_device *dev,
                                  rt_off_t          pos,
                                  void             *buffer,
                                  rt_size_t         size)
{
    return size;
}

static rt_size_t general_lcd_write(struct rt_device *dev,
                                   rt_off_t          pos,
                                   const void       *buffer,
                                   rt_size_t         size)
{
    return size;
}

static struct rt_device rt_lcd_device;
int rt_hw_lcd_init(void)
{
    rt_err_t ret = RT_EOK;

    ret = LCD_Init();
    if (ret != RT_EOK) {
        return RT_ERROR;
    }

    lcddev.LCD_RD_DATA    = LCD_RD_DATA;
    lcddev.LCD_WR_DATA = LCD_WR_DATA;
    lcddev.LCD_WR_REG = LCD_WR_REG;
    lcddev.LcdWriteDataMultiple = LcdWriteDataMultiple;
    lcddev.LcdReadDataMultiple  = LcdReadDataMultiple;
    lcddev.LCD_WriteReg = LCD_WriteReg;
    lcddev.LCD_ReadReg = LCD_ReadReg;
    lcddev.LCD_Init = LCD_Init;
    lcddev.LCD_DisplayOn  = LCD_DisplayOn;
    lcddev.LCD_DisplayOff = LCD_DisplayOff;
    lcddev.LCD_Clear      = LCD_Clear;
    lcddev.LCD_Fill = LCD_Fill;
    lcddev.LCD_Color_Fill = LCD_Color_Fill;
    lcddev.LCD_SetCursor  = LCD_SetCursor;
    lcddev.LCD_Set_Window = LCD_Set_Window;
    lcddev.LCD_DrawPoint  = LCD_DrawPoint;
    lcddev.LCD_Fast_DrawPoint = LCD_Fast_DrawPoint;
    lcddev.LCD_ReadPoint  = LCD_ReadPoint;
    lcddev.LCD_WriteRAM_Prepare = LCD_WriteRAM_Prepare;
    lcddev.LCD_WriteRAM   = LCD_WriteRAM;
    lcddev.LCD_ReadRAM    = LCD_ReadRAM;
    lcddev.LCD_Rst          = LCD_Rst;
    lcddev.LCD_Power_Control    = LCD_Power_Control;
#if defined(RT_USING_COMPONENTS_METER_MYGUI)
    register_lcd_dev ( &lcddev );
    return RT_EOK;
#else
    CRC_HandleTypeDef hcrc = {0};
    hcrc.Instance = CRC;
    HAL_CRC_Init(&hcrc);
    rt_lcd_device.type        = RT_Device_Class_Graphic;
    rt_lcd_device.rx_indicate = RT_NULL;
    rt_lcd_device.tx_complete = RT_NULL;
    rt_lcd_device.init        = general_lcd_init;
    rt_lcd_device.open        = general_lcd_open;
    rt_lcd_device.close       = general_lcd_close;
    rt_lcd_device.read        = general_lcd_read;
    rt_lcd_device.write       = general_lcd_write;
    rt_lcd_device.control     = RT_NULL;
    rt_lcd_device.user_data   = (void *)&lcddev;
    /* register a character device */
    return rt_device_register(&rt_lcd_device, "lcd", RT_DEVICE_FLAG_DEACTIVATE);
#endif
}
INIT_DEVICE_EXPORT(rt_hw_lcd_init);
































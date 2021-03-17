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
//-----------------MCU�� LCD�˿ڶ���----------------
#if defined(SOC_STM32F407ZG)
static uint32_t lcd_power = 21;
static uint32_t lcd_backlight = GET_PIN(B, 15);
static uint32_t lcd_rst = 22;
#else
static uint32_t lcd_power = 34;
static uint32_t lcd_backlight = 66;
static uint32_t lcd_rst = 59;
#endif
static SRAM_HandleTypeDef TFTSRAM_Handler;    //SRAM���(���ڿ���LCD)

static void LCD_Clear(rt_uint16_t color);
static void LCD_SetCursor(rt_uint16_t Xpos, rt_uint16_t Ypos);

static void delay_us(rt_uint32_t us)
{
    __IO uint32_t Delay = us * (SystemCoreClock / 8U / 1000000U);
    do {
        __NOP();
    } while (Delay --);
}
//����LCD��Ҫ����
//Ĭ��Ϊ����
static _lcd_dev lcddev;

//д�Ĵ�������
//regval:�Ĵ���ֵ
static void LCD_WR_REG(__IO rt_uint16_t regval)
{
    regval = regval;    //ʹ��-O2�Ż���ʱ��,����������ʱ
    LCD->LCD_REG = regval; //д��Ҫд�ļĴ������
}
//дLCD����
//data:Ҫд���ֵ
static void LCD_WR_DATA(__IO rt_uint16_t data)
{
    data = data;        //ʹ��-O2�Ż���ʱ��,����������ʱ
    LCD->LCD_RAM = data;
}

//��LCD����
//����ֵ:������ֵ
static  rt_uint16_t LCD_RD_DATA(void)
{
    __IO rt_uint16_t ram;           //��ֹ���Ż�
    ram = LCD->LCD_RAM;
    return ram;
}

static void LcdWriteDataMultiple(rt_uint16_t * pData, int NumItems)
{
    while (NumItems--) {
        // ... TBD by user
        *pData = *pData;        //ʹ��-O2�Ż���ʱ��,����������ʱ
        LCD->LCD_RAM = *pData++;
    }
}

static void LcdReadDataMultiple(rt_uint16_t * pData, int NumItems)
{
    while (NumItems--) {
        // ... TBD by user
    }
}

//д�Ĵ���
//LCD_Reg:�Ĵ�����ַ
//LCD_RegValue:Ҫд�������
static void LCD_WriteReg(rt_uint16_t LCD_Reg, rt_uint16_t LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;     //д��Ҫд�ļĴ������
    LCD->LCD_RAM = LCD_RegValue;//д������
}
//���Ĵ���
//LCD_Reg:�Ĵ�����ַ
//����ֵ:����������
static rt_uint16_t LCD_ReadReg(rt_uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);        //д��Ҫ���ļĴ������
    delay_us(5);
    return LCD_RD_DATA();       //���ض�����ֵ
}
//��ʼдGRAM
static void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = lcddev.wramcmd;
}
//LCDдGRAM
//RGB_Code:��ɫֵ
static void LCD_WriteRAM(rt_uint16_t RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;//дʮ��λGRAM
}

//��mdk -O1ʱ���Ż�ʱ��Ҫ����
//��ʱi
static void opt_delay(rt_uint8_t i)
{
    while (i--);
}
//��ȡ��ĳ�����ɫֵ
//x,y:����
//����ֵ:�˵����ɫ
static rt_uint16_t LCD_ReadPoint(rt_uint16_t x, rt_uint16_t y)
{
    rt_uint16_t r = 0, g = 0, b = 0;
    if (x >= lcddev.width || y >= lcddev.height)return 0; //�����˷�Χ,ֱ�ӷ���
    LCD_SetCursor(x, y);
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X1963 || lcddev.id == 0X1408)LCD_WR_REG(0X2E); //9341/3510/1963 ���Ͷ�GRAMָ��
    else if (lcddev.id == 0X5510)LCD_WR_REG(0X2E00); //5510 ���Ͷ�GRAMָ��
    r = LCD_RD_DATA();                              //dummy Read
    if (lcddev.id == 0X1963) {
        return r;                   //1963,1408ֱ�Ӷ��Ϳ���
    } else if (lcddev.id == 0X1408) {
        /**In all color modes, the Frame memory read is always 24 bits so there is no restriction on the length of parameters.
        If data is not transferred in units of pixels, the extra data is regarded as invalid. When this command is read via DSI
        or MDDI, dummy read operation is not performed.Ŀǰ��ȡ�������󣬲��ɹ�**/
        r = LCD_RD_DATA();
        return r;
    }
    opt_delay(2);
    r = LCD_RD_DATA();                              //ʵ��������ɫ
    //9341/NT35310/NT35510Ҫ��2�ζ���
    opt_delay(2);
    b = LCD_RD_DATA();
    g = r & 0XFF;   //����9341/5310/5510,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
    g <<= 8;
    return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); //ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
}
//LCD������ʾ
static void LCD_DisplayOn(void)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X1963 || lcddev.id == 0X1408)LCD_WR_REG(0X29); //������ʾ
    else if (lcddev.id == 0X5510)LCD_WR_REG(0X2900); //������ʾ
}
//LCD�ر���ʾ
static void LCD_DisplayOff(void)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X1963 || lcddev.id == 0X1408)LCD_WR_REG(0X28); //�ر���ʾ
    else if (lcddev.id == 0X5510)LCD_WR_REG(0X2800); //�ر���ʾ
}
//���ù��λ��(��RGB����Ч)
//Xpos:������
//Ypos:������
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
        if (lcddev.dir == 0) { //x������Ҫ�任
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
//����LCD���Զ�ɨ�跽��(��RGB����Ч)
//ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341),
//����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
//dir:0~7,����8������(���嶨���lcd.h)
//9341/5310/5510/1963��IC�Ѿ�ʵ�ʲ���
static void LCD_Scan_Dir(rt_uint8_t dir)
{
    rt_uint16_t regval = 0;
    rt_uint16_t dirreg = 0;
    rt_uint16_t temp;
    if ((lcddev.dir == 1 && lcddev.id != 0X1963) || (lcddev.dir == 0 && lcddev.id == 0X1963)) { //����ʱ����1963���ı�ɨ�跽������ʱ1963�ı䷽��
        switch (dir) { //����ת��
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
    if (lcddev.id == 0x9341 || lcddev.id == 0X5310 || lcddev.id == 0X5510 || lcddev.id == 0X1963 || lcddev.id == 0X1408) { //9341/5310/5510/1963,���⴦��
        switch (dir) {
            case L2R_U2D://������,���ϵ���
                regval |= (0 << 7) | (0 << 6) | (0 << 5);
                break;
            case L2R_D2U://������,���µ���
                regval |= (1 << 7) | (0 << 6) | (0 << 5);
                break;
            case R2L_U2D://���ҵ���,���ϵ���
                regval |= (0 << 7) | (1 << 6) | (0 << 5);
                break;
            case R2L_D2U://���ҵ���,���µ���
                regval |= (1 << 7) | (1 << 6) | (0 << 5);
                break;
            case U2D_L2R://���ϵ���,������
                regval |= (0 << 7) | (0 << 6) | (1 << 5);
                break;
            case U2D_R2L://���ϵ���,���ҵ���
                regval |= (0 << 7) | (1 << 6) | (1 << 5);
                break;
            case D2U_L2R://���µ���,������
                regval |= (1 << 7) | (0 << 6) | (1 << 5);
                break;
            case D2U_R2L://���µ���,���ҵ���
                regval |= (1 << 7) | (1 << 6) | (1 << 5);
                break;
        }
        if (lcddev.id == 0X5510)dirreg = 0X3600;
        else dirreg = 0X36;
        if ((lcddev.id != 0X5310) && (lcddev.id != 0X5510) && (lcddev.id != 0X1963) && (lcddev.id != 0X1408))regval |= 0X08; //5310/5510/1963����ҪBGR
        LCD_WriteReg(dirreg, regval);
        if (lcddev.id != 0X1963 && lcddev.id != 0X1408) { //1963�������괦��
            if (regval & 0X20) {
                if (lcddev.width < lcddev.height) { //����X,Y
                    temp = lcddev.width;
                    lcddev.width = lcddev.height;
                    lcddev.height = temp;
                }
            } else {
                if (lcddev.width > lcddev.height) { //����X,Y
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
//����
//x,y:����
//POINT_COLOR:�˵����ɫ
static void LCD_DrawPoint(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color)
{
    LCD_SetCursor(x, y);    //���ù��λ��
    LCD_WriteRAM_Prepare(); //��ʼд��GRAM
    LCD->LCD_RAM = color;
}
//���ٻ���
//x,y:����
//color:��ɫ
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
//SSD1963 ��������
//pwm:����ȼ�,0~100.Խ��Խ��.
static void LCD_SSD_BackLightSet(rt_uint8_t pwm)
{
    LCD_WR_REG(0xBE);   //����PWM���
    LCD_WR_DATA(0x05);  //1����PWMƵ��
    LCD_WR_DATA(pwm * 2.55); //2����PWMռ�ձ�
    LCD_WR_DATA(0x01);  //3����C
    LCD_WR_DATA(0xFF);  //4����D
    LCD_WR_DATA(0x00);  //5����E
    LCD_WR_DATA(0x00);  //6����F
}

//����LCD��ʾ����
//dir:0,������1,����
static void LCD_Display_Dir(rt_uint8_t dir)
{
    lcddev.dir = dir;   //����/����
    if (dir == 0) {     //����
        lcddev.width = 240;
        lcddev.height = 320;
        if (lcddev.id == 0X9341 || lcddev.id == 0X5310) {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
            if (lcddev.id == 0X5310) {
                lcddev.width = 320;
                lcddev.height = 480;
            } else if (lcddev.id == 0X1408) { //R61408Ĭ�ϵ�ɨ�跽ʽ������ɨ��
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
            lcddev.wramcmd = 0X2C;  //����д��GRAM��ָ��
            lcddev.setxcmd = 0X2B;  //����дX����ָ��
            lcddev.setycmd = 0X2A;  //����дY����ָ��
            lcddev.width = 480;     //���ÿ��480
            lcddev.height = 800;    //���ø߶�800
        } else if (lcddev.id == 0X1408) {
            lcddev.wramcmd = 0X2C;  //����д��GRAM��ָ��
            lcddev.setxcmd = 0X2A;  //����дX����ָ��
            lcddev.setycmd = 0X2B;  //����дY����ָ��
            lcddev.width = 480;     //���ÿ��480
            lcddev.height = 800;    //���ø߶�800
        }
    } else {             //����
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
            lcddev.wramcmd = 0X2C;  //����д��GRAM��ָ��
            lcddev.setxcmd = 0X2A;  //����дX����ָ��
            lcddev.setycmd = 0X2B;  //����дY����ָ��
            lcddev.width = 800;     //���ÿ��800
            lcddev.height = 480;    //���ø߶�480
        } else if (lcddev.id == 0X1408) {
            lcddev.wramcmd = 0X2C;  //����д��GRAM��ָ��
            lcddev.setxcmd = 0X2A;  //����дX����ָ��
            lcddev.setycmd = 0X2B;  //����дY����ָ��
            lcddev.width = 800;     //���ÿ��800
            lcddev.height = 480;    //���ø߶�480
        }
        if (lcddev.id == 0X5310) {
            lcddev.width = 480;
            lcddev.height = 320;
        }
    }
    LCD_Scan_Dir(DFT_SCAN_DIR); //Ĭ��ɨ�跽��
}
//���ô���(��RGB����Ч),���Զ����û������굽�������Ͻ�(sx,sy).
//sx,sy:������ʼ����(���Ͻ�)
//width,height:���ڿ�Ⱥ͸߶�,�������0!!
//�����С:width*height.
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
    } else if (lcddev.id == 0X1963) { //1963�������⴦��
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
//SRAM�ײ�������ʱ��ʹ�ܣ����ŷ���
//�˺����ᱻHAL_SRAM_Init()����
//hsram:SRAM���
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_FSMC_CLK_ENABLE();            //ʹ��FSMCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();           //ʹ��GPIODʱ��
    __HAL_RCC_GPIOE_CLK_ENABLE();           //ʹ��GPIOEʱ��
    __HAL_RCC_GPIOF_CLK_ENABLE();           //ʹ��GPIOFʱ��
#if defined(SOC_STM32F407ZG)
    __HAL_RCC_GPIOG_CLK_ENABLE();           //ʹ��GPIOFʱ��
#endif
#if !defined(SOC_STM32F407ZG)
    //��ʼ��PD0,1,4,5,8,9,10,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | \
                       GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
#else
    //��ʼ��PD0,1,4,5,8,9,10,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | \
                       GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_14 | GPIO_PIN_15;

#endif
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;    //���츴��
    GPIO_Initure.Pull = GPIO_PULLUP;        //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //����
    GPIO_Initure.Alternate = GPIO_AF12_FSMC; //����ΪFSMC
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);    //��ʼ��

    //��ʼ��PE7,8,9,10,11,12,13,14,15
    GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
                       GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);
    //��ʼ��PE7,8,9,10,11,12,13,14,15
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
//��ʼ��lcd
//�ó�ʼ���������Գ�ʼ�������ͺŵ�LCD(�����.c�ļ���ǰ�������)
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
    TFTSRAM_Handler.Init.NSBank = FSMC_NORSRAM_BANK1;                   //ʹ��NE1
#endif
    TFTSRAM_Handler.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE; //��ַ/�����߲�����
    TFTSRAM_Handler.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;            //SRAM
    TFTSRAM_Handler.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16; //16λ���ݿ��
    TFTSRAM_Handler.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE; //�Ƿ�ʹ��ͻ������,����ͬ��ͻ���洢����Ч,�˴�δ�õ�
    TFTSRAM_Handler.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; //�ȴ��źŵļ���,����ͻ��ģʽ����������
    TFTSRAM_Handler.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS; //�洢�����ڵȴ�����֮ǰ��һ��ʱ�����ڻ��ǵȴ������ڼ�ʹ��NWAIT
    TFTSRAM_Handler.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;  //�洢��дʹ��
    TFTSRAM_Handler.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;         //�ȴ�ʹ��λ,�˴�δ�õ�
    TFTSRAM_Handler.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;      //��дʹ�ò�ͬ��ʱ��
    TFTSRAM_Handler.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE; //�Ƿ�ʹ��ͬ������ģʽ�µĵȴ��ź�,�˴�δ�õ�
    TFTSRAM_Handler.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;         //��ֹͻ��д
    TFTSRAM_Handler.Init.ContinuousClock = FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;

    //FMC��ʱ����ƼĴ���
    FSMC_ReadWriteTim.AddressSetupTime = 0x0F + 10;     //��ַ����ʱ�䣨ADDSET��Ϊ16��HCLK 1/168M=6ns*16=96ns
    FSMC_ReadWriteTim.AddressHoldTime = 0;
    FSMC_ReadWriteTim.DataSetupTime = 60 + 40;          //���ݱ���ʱ��Ϊ60��HCLK    =6*60=360ns
    FSMC_ReadWriteTim.AccessMode = FSMC_ACCESS_MODE_A; //ģʽA
    //FMCдʱ����ƼĴ���
    FSMC_WriteTim.BusTurnAroundDuration = 0;        //������ת�׶γ���ʱ��Ϊ0���˱�������ֵ�Ļ���Ī��������Զ��޸�Ϊ4�����³�����������
    FSMC_WriteTim.AddressSetupTime = 9;             //��ַ����ʱ�䣨ADDSET��Ϊ9��HCLK =54ns
    FSMC_WriteTim.AddressHoldTime = 0;
    FSMC_WriteTim.DataSetupTime = 8;                //���ݱ���ʱ��Ϊ6ns*9��HCLK=54n
    FSMC_WriteTim.AccessMode = FSMC_ACCESS_MODE_A;  //ģʽA
    HAL_SRAM_Init(&TFTSRAM_Handler, &FSMC_ReadWriteTim, &FSMC_WriteTim);

    rt_pin_mode  (lcd_power, PIN_MODE_OUTPUT);
    rt_pin_mode  (lcd_backlight, PIN_MODE_OUTPUT);
    rt_pin_mode  (lcd_rst, PIN_MODE_OUTPUT);

    rt_pin_write(lcd_power, GPIO_PIN_RESET);//��LCD��Դ
    HAL_Delay(1); // delay 5 ms
    LCD_Rst();


    //����9341 ID�Ķ�ȡ
    LCD_WR_REG(0XD3);
    lcddev.id = LCD_RD_DATA();  //dummy read
    lcddev.id = LCD_RD_DATA();  //����0X00
    lcddev.id = LCD_RD_DATA();  //��ȡ93
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA(); //��ȡ41
    if (lcddev.id != 0X9341) {  //��9341,���Կ����ǲ���NT35310
        LCD_WR_REG(0XD4);
        lcddev.id = LCD_RD_DATA(); //dummy read
        lcddev.id = LCD_RD_DATA(); //����0X01
        lcddev.id = LCD_RD_DATA(); //����0X53
        lcddev.id <<= 8;
        lcddev.id |= LCD_RD_DATA(); //�������0X10
        if (lcddev.id != 0X5310) {  //Ҳ����NT35310,���Կ����ǲ���NT35510
            LCD_WR_REG(0XDA00);
            lcddev.id = LCD_RD_DATA();      //����0X00
            LCD_WR_REG(0XDB00);
            lcddev.id = LCD_RD_DATA();      //����0X80
            lcddev.id <<= 8;
            LCD_WR_REG(0XDC00);
            lcddev.id |= LCD_RD_DATA();     //����0X00
            if (lcddev.id == 0x8000)lcddev.id = 0x5510; //NT35510���ص�ID��8000H,Ϊ��������,����ǿ������Ϊ5510
            if (lcddev.id != 0X5510) {      //Ҳ����NT5510,���Կ����ǲ���SSD1963
                LCD_WR_REG(0XA1);   //�˼Ĵ������κ�ģʽ�¶��ɶ�ȡ
                lcddev.id = LCD_RD_DATA();
                lcddev.id = LCD_RD_DATA();  //����0X57
                lcddev.id <<= 8;
                lcddev.id |= LCD_RD_DATA(); //����0X61
                if (lcddev.id == 0X5761) {
                    lcddev.id = 0X1963; //SSD1963���ص�ID��5761H,Ϊ��������,����ǿ������Ϊ1963
                } else { //���������ݣ���ǿ����оƬ����0x1408
                    lcddev.id = 0X1408;
                }
            }
        }
    }
//  printf(" LCD ID:%x\r\n",lcddev.id); //��ӡLCD ID
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
//      WriteData(0x60);    //���ǵ�������ʱ���ҵ���Y�������ϵ��£�X��
        LCD_WR_DATA(0x00);  //���ǵ�������ʱ�����ң����ϵ���
    } else if (lcddev.id == 0X9341) { //9341��ʼ��
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
        LCD_SSD_BackLightSet(100);//��������Ϊ����
    }

    //��ʼ������Ժ�,����
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X5510 || lcddev.id == 0X1963 || lcddev.id == 0X1408) { //������⼸��IC,������WRʱ��Ϊ���
        //��������дʱ����ƼĴ�����ʱ��
        FSMC_Bank1E->BWTR[6] &= ~(0XF << 0); //��ַ����ʱ��(ADDSET)����
        FSMC_Bank1E->BWTR[6] &= ~(0XF << 8); //���ݱ���ʱ������
        FSMC_Bank1E->BWTR[6] |= 3 << 0; //��ַ����ʱ��(ADDSET)Ϊ3��HCLK =18ns
        FSMC_Bank1E->BWTR[6] |= 2 << 8; //���ݱ���ʱ��(DATAST)Ϊ6ns*3��HCLK=18ns
    }
    LCD_Display_Dir(0);     //Ĭ��Ϊ����
    rt_pin_write(lcd_backlight, GPIO_PIN_SET); //��������,���ڸ�ΪPWM����
    LCD_Clear(BLUE);
	
    return RT_EOK;
}

//��������
//color:Ҫ���������ɫ
static void LCD_Clear(rt_uint16_t color)
{
    rt_uint32_t index = 0;
    rt_uint32_t totalpoint = lcddev.width;
    totalpoint *= lcddev.height;        //�õ��ܵ���
    LCD_SetCursor(0, 0);    //���ù��λ��
    LCD_WriteRAM_Prepare();             //��ʼд��GRAM
    for (index = 0; index < totalpoint; index++) {
        LCD->LCD_RAM = color;
    }
}
//��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
//color:Ҫ������ɫ
static void LCD_Fill(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint16_t color)
{
    rt_uint16_t i, j;
    rt_uint16_t xlen = 0;
    xlen = ex - sx + 1;
    for (i = sy; i <= ey; i++) {
        LCD_SetCursor(sx, i);                   //���ù��λ��
        LCD_WriteRAM_Prepare();                 //��ʼд��GRAM
        for (j = 0; j < xlen; j++)LCD->LCD_RAM = color; //��ʾ��ɫ
    }
}
//��ָ�����������ָ����ɫ��
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
//color:Ҫ������ɫ
static void LCD_Color_Fill(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint16_t *color)
{
    rt_uint16_t height, width;
    rt_uint16_t i, j;
    width = ex - sx + 1;    //�õ����Ŀ��
    height = ey - sy + 1;   //�߶�
    for (i = 0; i < height; i++) {
        LCD_SetCursor(sx, sy + i);  //���ù��λ��
        LCD_WriteRAM_Prepare();     //��ʼд��GRAM
        for (j = 0; j < width; j++)LCD->LCD_RAM = color[i * width + j]; //д������
    }
}

static rt_uint16_t LCD_ReadRAM ( void )
{
    return 0;
}

static void LCD_Power_Control(LCD_Power_Type eState)
{
    if (eState == LCD_POWER_ON) {
        rt_pin_write(lcd_power, GPIO_PIN_RESET);//��LCD��Դ
    } else {
        rt_pin_write(lcd_power, GPIO_PIN_SET);//��LCD��Դ
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
































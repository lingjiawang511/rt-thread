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
//LCD��ַ�ṹ��
typedef struct {
    __IO rt_uint16_t LCD_REG;
    __IO rt_uint16_t LCD_RAM;
} LCD_TypeDef;
//ʹ��NOR/SRAM�� Bank1.sector1,��ַλHADDR[27,26]=00 A16��Ϊ��������������
//ע������ʱSTM32�ڲ�������һλ����! 1 1111 1111 1 111 1110=0x1FFFE
#if defined(SOC_STM32F407ZG)
#define LCD_BASE         ((rt_uint32_t)(0x6C000000 | 0x0000007E))
#else
#define LCD_BASE         ((rt_uint32_t)(0x60000000 | 0x0001FFFE))
#endif
#define LCD             ((LCD_TypeDef *) LCD_BASE)
//////////////////////////////////////////////////////////////////////////////////

//ɨ�跽����
#define L2R_U2D  0      //������,���ϵ���
#define L2R_D2U  1      //������,���µ���
#define R2L_U2D  2      //���ҵ���,���ϵ���
#define R2L_D2U  3      //���ҵ���,���µ���

#define U2D_L2R  4      //���ϵ���,������
#define U2D_R2L  5      //���ϵ���,���ҵ���
#define D2U_L2R  6      //���µ���,������
#define D2U_R2L  7      //���µ���,���ҵ���

#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�ϵ�ɨ�跽��

//������ɫ
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
#define BROWN            0XBC40 //��ɫ
#define BRRED            0XFC07 //�غ�ɫ
#define GRAY             0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE         0X01CF //����ɫ
#define LIGHTBLUE        0X7D7C //ǳ��ɫ
#define GRAYBLUE         0X5458 //����ɫ
//������ɫΪPANEL����ɫ

#define LIGHTGREEN       0X841F //ǳ��ɫ
//#define LIGHTGRAY        0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY            0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

//LCD�ֱ�������
#define SSD_HOR_RESOLUTION      800     //LCDˮƽ�ֱ���
#define SSD_VER_RESOLUTION      480     //LCD��ֱ�ֱ���
//LCD������������
#define SSD_HOR_PULSE_WIDTH     1       //ˮƽ����
#define SSD_HOR_BACK_PORCH      46      //ˮƽǰ��
#define SSD_HOR_FRONT_PORCH     210     //ˮƽ����

#define SSD_VER_PULSE_WIDTH     1       //��ֱ����
#define SSD_VER_BACK_PORCH      23      //��ֱǰ��
#define SSD_VER_FRONT_PORCH     22      //��ֱǰ��
//���¼����������Զ�����
#define SSD_HT  (SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS (SSD_HOR_BACK_PORCH)
#define SSD_VT  (SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

typedef struct {
    rt_uint16_t width;          //LCD ���
    rt_uint16_t height;         //LCD �߶�
    rt_uint16_t id;             //LCD ID
    rt_uint8_t  dir;            //���������������ƣ�0��������1��������
    rt_uint16_t wramcmd;        //��ʼдgramָ��
    rt_uint16_t setxcmd;        //����x����ָ��
    rt_uint16_t setycmd;        //����y����ָ��

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






#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <rtthread.h>
#include <drivers/i2c.h>
#include "drv_i2c.h"
#include "include/HeadType.h"

#define DEBUG_FLASH 0x00

#define AT24C01  (127)
#define AT24C02  (255)
#define AT24C04  (511)
#define AT24C08  (1023)
#define AT24C16  (2047)
#define AT24C32  (4095)
#define AT24C64  (8191)
#define AT24C128 (16383)
#define AT24C256 (32767)

#define EE_TYPE  (AT24C02)

#define AT24CXX_I2C_BUS_NAME    ("i2c2")
struct rt_i2c_bus_device *at24cxx_i2c_bus = RT_NULL;

const uint8_t AT24XXX_ADDR = 0xA0 >> 1;

static TagParam *pLocal;
static long *pParam;
static UL flash_op;
uint16_t nLenByte    = (sizeof(*pLocal) - GET_OFFSET(TagParam, dot_num));
uint16_t backup_offset = (sizeof(*pLocal) - GET_OFFSET(TagParam, dot_num));

static void eeprom_write_byte(uint16_t write_addr, uint8_t dt)
{
    struct rt_i2c_msg msg[1] = {0};
    unsigned char buf[2] = {0};

    if (EE_TYPE > AT24C02) {
        buf[0] = (write_addr >> 8) & 0xff;
        buf[1] = (write_addr) & 0xff;
        buf[2] = dt;
        msg[0].addr    = AT24XXX_ADDR;
        msg[0].flags   = RT_I2C_WR;
        msg[0].buf     = buf;
        msg[0].len     = 3;
    } else {
        buf[0] = write_addr;
        buf[1] = dt;
        msg[0].addr    = AT24XXX_ADDR;
        msg[0].flags   = RT_I2C_WR;
        msg[0].buf     = buf;
        msg[0].len     = 2;
    }
    rt_i2c_transfer(at24cxx_i2c_bus, msg, 1);
}

uint16_t crc16(uint8_t *puchMsg, uint16_t usDataLen)
{
    uint8_t uchCRCHi = 0xFF ; /* 高CRC字节初始化 */
    uint8_t uchCRCLo = 0xFF ; /* 低CRC 字节初始化 */
    uint8_t uIndex ; /* CRC循环中的索引 */
    while (usDataLen--) { /* 传输消息缓冲区 */
        uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex] ;
    }
    return (uchCRCHi << 8 | uchCRCLo) ;
}

/* write one log in general */
static void eeprom_write_uint32(uint16_t WriteAddr, uint32_t DataToWrite, uint8_t Len)
{
    uint8_t t;
    for (t = 0; t < Len; t++) {
        eeprom_write_byte(WriteAddr + t, (DataToWrite >> (8 * t)) & 0xff);
    }
}

/* wirte a buffer in general */
static void eeprom_write_buffer(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite)
{
    while (NumToWrite--) {
        eeprom_write_byte(WriteAddr, *pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}

uint8_t eeprom_read_byte(uint16_t addr)
{
    uint8_t rdata;
    uint8_t read_addr[2];

    struct rt_i2c_msg msgs[3];
    unsigned char data = 0;

    read_addr[0] = 0xFF & (addr >> 8);
    read_addr[0] = 0xFF & (addr);

    msgs[0].addr    = AT24XXX_ADDR;
    msgs[0].flags   = RT_I2C_WR;
    msgs[0].buf     = &read_addr[0];
    msgs[0].len     = 1;

    if (EE_TYPE > AT24C02) {
        msgs[1].addr    = AT24XXX_ADDR;
        msgs[1].flags   = RT_I2C_WR;
        msgs[1].buf     = &read_addr[1];
        msgs[1].len     = 1;

        msgs[2].addr    = AT24XXX_ADDR;
        msgs[2].flags   = RT_I2C_RD;
        msgs[2].buf     = &rdata;
        msgs[2].len     = 1;
        rt_i2c_transfer(at24cxx_i2c_bus, msgs, 3);
    } else {
        msgs[1].addr    = AT24XXX_ADDR;
        msgs[1].flags   = RT_I2C_RD;
        msgs[1].buf     = &rdata;
        msgs[1].len     = 1;
        rt_i2c_transfer(at24cxx_i2c_bus, msgs, 2);
    }

    return rdata;
}

uint8_t sequential_write_flash_in_one_sector(WORD waddr, WORD wdatalen, uint8_t array[])
{
    uint16_t i;
    static uint8_t wmode = 1;
    UL datatemp;
    i = waddr;
    if (1 == wmode) {
        while (wdatalen > 0) {
            wdatalen -= 4;
            flash_op.c[0] = eeprom_read_byte(i++);
            flash_op.c[1] = eeprom_read_byte(i++);
            flash_op.c[2] = eeprom_read_byte(i++);
            flash_op.c[3] = eeprom_read_byte(i++);
            datatemp.c[0] =  *array++;
            datatemp.c[1] =  *array++;
            datatemp.c[2] =  *array++;
            datatemp.c[3] =  *array++;
            if (i % 4 == 0) {
                if (flash_op.l == datatemp.l) {
                    waddr += 4;
                } else {
                    eeprom_write_uint32(waddr, datatemp.l, 4);
                    waddr += 4;
                }

            }
        }
    } else {
        eeprom_write_buffer(waddr, array, wdatalen);
    }
    return (uint8_t)(1);
}

/* there is an implement of crc16 on modbus.c */

static unsigned char check_crc(uchar *pMsg, uint16_t usDataLen)
{
    uint16_t crc;
    crc = usDataLen - 4;
    flash_op.c[0] = pMsg[crc++];
    flash_op.c[1] = pMsg[crc++];
    flash_op.c[2] = pMsg[crc++];
    flash_op.c[3] = pMsg[crc];
    crc = crc16(pMsg, (usDataLen - 4));
    if (flash_op.l == (crc)) {
        return 1;
    }
    return 0;
}

long read_param(uchar index)
{
    uint16_t i;
    i = (0x004 * index);
    flash_op.c[0] = eeprom_read_byte(i++);
    flash_op.c[1] = eeprom_read_byte(i++);
    flash_op.c[2] = eeprom_read_byte(i++);
    flash_op.c[3] = eeprom_read_byte(i);
    return (flash_op.l);
}

long read_param_backup(uchar index)
{
    uint16_t i;
    i = (0x004 * index) + backup_offset;
    flash_op.c[0] = eeprom_read_byte(i++);
    flash_op.c[1] = eeprom_read_byte(i++);
    flash_op.c[2] = eeprom_read_byte(i++);
    flash_op.c[3] = eeprom_read_byte(i);
    return (flash_op.l);
}


#define write_param() do\
{\
    sequential_write_flash_in_one_sector(0x00,nLenByte,(uint8_t *)(&pLocal->dot_num.l));\
    sequential_write_flash_in_one_sector(backup_offset,nLenByte,(uint8_t *)(&pLocal->dot_num.l));\
}while(0)

bool flush_flash(void)
{
    pLocal->crc.l = crc16((uint8_t *)(&pLocal->dot_num.l), (nLenByte - 4)); //caculate the crc value
    write_param(); //前面的3个参数不用保存
    return 0;
}

void Param_Factory(TagParam *p)
{
    /* bellow is ram only param */
    p->pv.l = 0; //0，TFT显示值
    p->av.l = 0; //2，从ADS1232读出的值
    p->temp_value.l = 0;
    p->humi_value.l = 0;
    p->drew_value.l = 0;
    p->ppmv_value.l = 0;
    p->op.l = 0;
    /* bellow is eeprom param */
    p->dot_num.l = 2;
    p->unit_name.l = 0;
    p->language.l = LANGUAGE_ENGLISH;
    p->keytone.l = 0;
    p->lcd_backight.l = 50;
    p->lcd_contrast.l = 40;
    p->display_mode.l = 0;
    p->display_param.l = 4;
    /* communication param */
    p->address.l = 1;
    p->baudrate.l = 19200;
    p->protocol.l = 0;
    p->probe_addr.l = 1;
    p->probe_baudrate.l = 19200;
    p->probe_use_chksum.l = CHKSUM;
    /* dynamic menu enable param */
    /*恢复出厂设置不允许改变菜单使能项*/
//      p->curve_enable.l = 1;
//      p->communication_enable.l = 1;
//      p->analog_out_enable.l = 1;
//      p->digital_out_enable.l = 1;
//      p->humi_enable.l = 1;
//      p->temp_enable.l = 1;
//      p->time_enable.l = 1;
//      p->fastdew_enable.l = 1;
    /* dac relate params */
#if USE_DAC_OUTPUT == 1
    p->dac_out[DAC_CH1][OUTPUT_CH].c[DAC_CHANNEL] = 0;
    p->dac_out[DAC_CH1][OUTPUT_CH].c[DAC_SWITCH] = 0;
    p->dac_out[DAC_CH1][OUTPUT_CH].c[DAC_OUTTYPE] = 0;
    p->dac_out[DAC_CH1][IN_ZERO].l = 0;
    p->dac_out[DAC_CH1][IN_FULL].l = 0;
    p->dac_out[DAC_CH1][CALIB_MIN_1].l = 2920;
    p->dac_out[DAC_CH1][CALIB_MAX_1].l = 14650;
    p->dac_out[DAC_CH1][CALIB_MIN_2].l = 0;
    p->dac_out[DAC_CH1][CALIB_MAX_2].l = 15130;
    p->dac_out[DAC_CH2][OUTPUT_CH].c[DAC_CHANNEL] = 1;
    p->dac_out[DAC_CH2][OUTPUT_CH].c[DAC_SWITCH] = 0;
    p->dac_out[DAC_CH2][OUTPUT_CH].c[DAC_OUTTYPE] = 0;
    p->dac_out[DAC_CH2][IN_ZERO].l = 0;
    p->dac_out[DAC_CH2][IN_FULL].l = 0;
    p->dac_out[DAC_CH2][CALIB_MIN_1].l = 2920;
    p->dac_out[DAC_CH2][CALIB_MAX_1].l = 14750;
    p->dac_out[DAC_CH2][CALIB_MIN_2].l = 0;
    p->dac_out[DAC_CH2][CALIB_MAX_2].l = 15130;
#endif
    /* alarm relate params */
    p->auto_alarm.l = 0 ;
    p->alarm_out[0][ALARM_CH].l = 0;
    p->alarm_out[0][ALARM_HIGH].l = 6000;
    p->alarm_out[0][ALARM_LOW].l = 0;
    p->alarm_out[0][ALARM_DELAYON].l = 0;
    p->alarm_out[0][ALARM_DELAYOFF].l = 0;
    p->alarm_out[1][ALARM_CH].l = 1;
    p->alarm_out[1][ALARM_HIGH].l = 6000;
    p->alarm_out[1][ALARM_LOW].l = 0;
    p->alarm_out[1][ALARM_DELAYON].l = 0;
    p->alarm_out[1][ALARM_DELAYOFF].l = 0;
    /* digital input sensor params */
    p->ptotal.l = 101330;
    p->dewpoint_skip_time.l = 0;
    p->HCA_HUMI_ACQUIRED.l = 0;
    p->HCA_HUMI_DELETE.l = 0;
    p->HCA_HUMI_ADJUST.l = 0;
    p->HCA_TEMP_ADJUST.l = 0;
    p->calibration_count.l = 2;
    p->trend_funtion.l = 1;
    /* ananalog input relate param */
    p->load.l = 0;
    p->band.l = 5;
    p->band_base.l = 200;
    p->fitl.l = 3;
    p->zero_value.l = 5;//10,zero driftting
    p->zero_time.l = 0;
    p->display_offset.l = 0;
    if ((p->calibramax.l < 2) || (p->calibramax.l > 8)) {
        p->calibramax.l = 2;
    }
    p->a_av[0].l = -1279;
    p->a_av[1].l = 4298000;
    p->a_av[2].l = 0;
    p->a_av[3].l = 0;
    p->a_av[4].l = 0;
    p->a_av[5].l = 0;
    p->a_pv[0].l = 0;
    p->a_pv[1].l = 10000;
    p->a_pv[2].l = 12000;
    p->a_pv[3].l = 14000;
    p->a_pv[4].l = 16000;
    p->a_pv[5].l = 20000;
    /* curve param */
    p->curve_channel.l = 0;     //曲线只显示一条曲线时，选择显示哪一个变量，本仪表也不用
    p->curve_dis_mode.l = 3; //曲线显示模式
    p->graph_x.l = 3;
    p->graph_y1.l = 0;
    p->graph_y2.l = 1000000;

    //DEBUG lang     = (uint8_t)(p->language.l) ? LANGUAGE_CHINESE : LANGUAGE_ENGLISH;
    //DEBUG key_tone = (uint8_t)(p->keytone.l) ? 1 : 0;
    //DEBUG LCD_Backlight_Control((uint8_t)p->lcd_backight.l);
    //DEBUG FastSecPerOnedays = 5; //这个值出厂前需要我们校准给出，菜单开放给客户改，因为温度会影响RTC，不同的客户可以微调
}

static void Param_Loading_Init(TagParam *p)
{
    /**this Param only need init when the EEPROM param calculate CRC err**/
    p->curve_enable.l = 1;
    p->communication_enable.l  = 1;
    p->analog_out_enable.l  = 3;
    p->digital_out_enable.l  = 3;
    p->humi_enable.l  = 1;
    p->temp_enable.l  = 1;
    p->time_enable.l  = 0;
    /**this Param only need init when the EEPROM param calculate CRC err**/
    Param_Factory(p);
}

void InitParam(TagParam *p)
{
    unsigned char buffer[sizeof(*p)];
    int i;
    int len;
    pLocal = p;
    len =  sizeof(*p) / sizeof(long) - (3 + 4); //前面的3个参数不用读取
    pParam = &(p->dot_num.l);
    for (i = 0; i < nLenByte; i++) {
        buffer[i] =  eeprom_read_byte(i);
    }
    if (!check_crc(buffer, nLenByte)) { //check normal section
        for (i = 0; i < nLenByte; i++) {
            buffer[i] =  eeprom_read_byte((backup_offset + i));
        }
        if (!check_crc(buffer, nLenByte)) { //check backup section
//            Param_Factory(p); //get factory value
            Param_Loading_Init(p);
            //DEBUG Curser = NULL;
            return;
        } else {
            for (i = 0; i < len; i++) { //0,1,2 is not in the eeprom
                pParam[i] = read_param_backup(i);
            }
        }
    } else { //check normal section,OK ...
        for (i = 0; i < len; i++) { //0,1,2 is not in the eeprom
            pParam[i] = read_param(i);
        }
    }
    //DEBUG Curser = NULL;
    //DEBUG lang     = (uint8_t)(p->language.l) ? LANGUAGE_CHINESE : LANGUAGE_ENGLISH;
    //DEBUG key_tone = (uint8_t)(p->keytone.l) ? 1 : 0;
    //DEBUG Curve.curve_mode = (uint8_t)p->curve_dis_mode.l;
    //DEBUG ModifyMenuData.menu_dot_Flag = 0;
    //DEBUG ModifyMenuData.calibraFlag = 0;
    //DEBUG SensorOnlineFlag = 0;
    /*初始化pParam[i] = read_param(i);的时候导致ModifyMenuData.calibraFlag = 0xff,可能有隐藏的BUG*/
    if ((p->display_param.l < 2) || (p->display_param.l > 4)) {
        p->display_param.l = 4;
    }
}

#include <rtthread.h>

/**
* \brief eeprom ??????
*/

#define RSA_MB_POLL_LENGTH (40)

typedef struct {
    struct rt_mailbox mb;
    char mb_pool[RSA_MB_POLL_LENGTH];
} tag_mailbox;

static tag_mailbox mailbox;

static void thread_entry_eeprom (void* parameter)
{
    uint32_t val = 0;

    at24cxx_i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(AT24CXX_I2C_BUS_NAME);
    if (RT_NULL == at24cxx_i2c_bus)    {
        rt_kprintf("[%s] no i2c device(at24cxx), therad exit...\n", __FUNCTION__);
        return ;
    }

    rt_mb_init(&mailbox.mb, "mbt", &(mailbox.mb_pool[0]), sizeof(mailbox.mb_pool) / 4, RT_IPC_FLAG_FIFO);

    //InitParam(rt_get_param_addr());

    while (1) {
        if (rt_mb_recv(&mailbox.mb, (rt_uint32_t*)&val, RT_WAITING_FOREVER) == RT_EOK) {
            rt_kprintf("get eeprom message, the content:%d\n", val);
        }
    }
}

static void eeprom_read_write_test(void)
{
    uint8_t val = 0;

    rt_kprintf("before read val = %d\n", val);
    val = eeprom_read_byte(0x00);
    rt_kprintf("after read val = %d\n", val);
    val++;
    eeprom_write_byte(0x00, val);
}

long eeprom_test(int which)
{
    switch (which) {
        case 0:
            eeprom_read_write_test();
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            rt_mb_send(&(mailbox.mb), (rt_uint32_t)which);
            break;
        default:
            rt_kprintf("Invalid test case\n");
            return -1;
    }

    return 0;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(eeprom_test, perform eeprom  test);
#endif

static rt_uint8_t thread_eeprom_handle_stack[512];
static struct rt_thread thread_eeprom_handle;

static int eeprom_init(void)
{
    rt_thread_init(&thread_eeprom_handle, "eeprom_task",
                   thread_entry_eeprom, RT_NULL, thread_eeprom_handle_stack,
                   sizeof(thread_eeprom_handle_stack), 10,
                   5);
    rt_thread_startup(&thread_eeprom_handle);
}

INIT_COMPONENT_EXPORT(eeprom_init);

#include "auchCRC16.h"

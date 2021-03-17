#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CHKSUM 0

#ifndef false
#define false 0
#define true (!(false))
#endif

typedef enum {
    fsm_rt_err     = -1,
    fsm_rt_cpl     = 0,
    fsm_rt_on_going = 1
} fsm_rt_t;
typedef union {
    long l;
    unsigned long ul;
    int  i;
    unsigned int ui;
    short si[2];
    char  c[4];
    unsigned char uc[4];
} UL;
typedef union {
    short i;
    unsigned short ui;
    char c[2];
    unsigned char uc[2];
} UI;
typedef  enum {
    NO_DATA_TREND,
    GENTLY_TREND,
    UP_TREND,
    DOWN_TREND,
} trend_state;
typedef  enum {
    HCA_HUMI_ACQUIRED,
    HCA_HUMI_DELETE,
    HCA_HUMI_ADJUST,
    HCA_TEMP_ADJUST,
} sensor_command_type;

typedef  enum {
    ALARM_CH,
    ALARM_HIGH,
    ALARM_LOW,
    ALARM_DELAYON,
    ALARM_DELAYOFF,
    ALARM_BUF_NUM,
} alarm_out_type;
typedef  enum {
    DAC_CH1,
    DAC_CH2,
    DAC_CHMAX,
} dac_channel_type;
typedef  enum {
    DAC_CHANNEL,
    DAC_SWITCH,
    DAC_OUTTYPE,
    DAC_bare,
    DAC_PARAMMAX,
} dac_param_type;
typedef  enum {
    OUTPUT_CH,
    IN_ZERO,
    IN_FULL,
    CALIB_MIN_1,
    CALIB_MAX_1,
    CALIB_MIN_2,
    CALIB_MAX_2,
    DAC_BUF_NUM,
} dac_out_type;
typedef struct {
    long display_index;
    long lenv_value;
    long temp_value;
    long humi_value;
    long drew_value;
    long ppmv_value;
} TagRuntime;
typedef struct {
    /* bellow is ram only param */
    UL pv; //0£¬TFTÏÔÊ¾Öµ
    UL av; //2£¬´ÓADS1232¶Á³öµÄÖµ
    UL temp_value;
    UL humi_value;
    UL drew_value;
    UL ppmv_value;
    UL op; //4 operation value
    /* bellow is eeprom param */
    UL dot_num;
    UL unit_name;
    UL language;
    UL keytone;
    UL lcd_backight;
    UL lcd_contrast;
    UL display_mode;
    UL display_param;
    /* communication param */
    UL address;   //6
    UL baudrate;  //8
    UL protocol;
    UL probe_addr;
    UL probe_baudrate;
    UL probe_use_chksum;
    /* dynamic menu enable param */
    UL curve_enable;
    UL communication_enable;
    UL analog_out_enable;
    UL digital_out_enable;
    UL humi_enable;
    UL temp_enable;
    UL time_enable;
    UL fastdew_enable;
    /* dac relate params */
    UL dac_out[DAC_CHMAX][DAC_BUF_NUM];
    /* alarm relate params */
    UL auto_alarm;
    UL alarm_out[2][ALARM_BUF_NUM];
    /* digital input sensor params */
    UL ptotal;
    UL dewpoint_skip_time;
    UL HCA_HUMI_ACQUIRED;
    UL HCA_HUMI_DELETE;
    UL HCA_HUMI_ADJUST;
    UL HCA_TEMP_ADJUST;
    UL calibration_count;
    UL trend_funtion;
    /* ananalog input relate param */
    UL load;
    UL band;
    UL band_base;
    UL fitl;
    UL zero_value;//10,zero driftting
    UL zero_time;
    UL display_offset;//12
    UL calibramax;//14
    UL a_av[6];//16£¬Ð£×¼Ê±µÚÒ»µãÊäÈëÖµ
    UL a_pv[6];//32£¬ÏÔÊ¾Ð£×¼Ê±µÚÒ»µãÊäÈëÖµ
    /* curve param */
    UL curve_channel;     //ÇúÏßÖ»ÏÔÊ¾Ò»ÌõÇúÏßÊ±£¬Ñ¡ÔñÏÔÊ¾ÄÄÒ»¸ö±äÁ¿£¬±¾ÒÇ±íÒ²²»ÓÃ
    UL curve_dis_mode; //ÇúÏßÏÔÊ¾Ä£Ê½
    UL graph_x; //40
    UL graph_y1; //42
    UL graph_y2; //42
    UL crc;//68
} TagParam;

static TagParam    param = {
	.ptotal = 101330,
};

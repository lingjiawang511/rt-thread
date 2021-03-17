#ifndef __HEADTYPE_H_
#define __HEADTYPE_H_

#include "ff.h"
//#include "malloc.h"
#include "exfuns.h"
#include "fattester.h"
#include "rsa.h"
#include "base64.h"
#include "myType.h"
#define ENABLE_MCUADC  0

#define LCD_TFT1 0
#define PWM_PATTERN_NUM1     100
#define MAX_LOG_FILE_NUM     100
#define MAX_LOG_DIR_NUM      365
#define SENSOR_ERR_NUM              20
#define USE_DAC_OUTPUT              1
#define USE_NEW_MENU          1
#define USE_MENU_CONFIG_KEY   1
#define CURVE_DATA_LOG_NUM     4
#define COMM_CALIBRATION_TIME   10   //单位S
#define INVALID_GPIO 0xFFFFFFFF

#define MAX_ROOT_COUNT (7)

#define KeyData    (PF_IDR)
#define smg_select (PF_IDR)

#define  DO        (PF_IDR)
#define  CK        (PF_IDR)
#define  KeySelect (PF_IDR)
#define  BEEP_OFF  gpio_set_value(jc_board_gpio_pin.beep,0)
#define  BEEP_ON   gpio_set_value(jc_board_gpio_pin.beep,1)

#define false      (0)
#define true       (!false)
#define TIME_60S   (6000)
#define TIME_100MS (10)
#define TIME_500MS (5*TIME_100MS)
#define TIME_2_5_SECONDS (250)

#ifndef NULL
#define NULL  ((void *)(0))
#endif

#define RE485_RECV   gpio_set_value(jc_board_gpio_pin.re_485,0)
#define RE485_SEND   gpio_set_value(jc_board_gpio_pin.re_485,1)
#define GET_OFFSET(Type, member) ((size_t)&(((Type*)0)->member))

#define LCD_PWM_USING_TIM3
#define LCD_PWM_USING_TIM4
#define CHECK_BATT_TIME   200    //200*10MS = 2S

typedef enum {
    EVENT_INVALID = 0,
    EVENT_CALI = 1, //calibraion_event,
    EVENT_ADDR = 2, //fix_addr_event_t,
    EVENT_BAUD = 3, //fix_baud_rate_t,
    EVENT_ZERO = 4, //fix_zero_value_t,
    EVENT_OFFET = 5, //fix_dsp_offset_t
    EVENT_FLASH = 6, //fix_dsp_offset_t
    EVENT_CALI_AV1 = 11, //calibraion_event
    EVENT_CALI_AV2 = 12, //calibraion_event
    EVENT_CALI_AV3 = 13, //calibraion_event
    EVENT_CALI_AV4 = 14, //calibraion_event
    EVENT_CALI_AV5 = 15, //calibraion_event
    EVENT_CALI_AV6 = 16, //calibraion_event
    EVENT_CALI_AV7 = 17, //calibraion_event
    EVENT_CALI_AV8 = 18, //calibraion_event
    EVENT_DO_FACTORY_RESET = 0x8000 //calibraion_event
} enum_event;

typedef enum {
    JC_COMM_RS485 = 0,
    JC_COMM_HID   = 1,
    JC_COMM_MAX
} jc_comm_type;

typedef enum {
    JC_BOARD_DIN_REV2 = 0,
    JC_BOARD_DIN_REV1 = 1,
    JC_BOARD_AIN_REV2 = 2,
    JC_BOARD_AIN_REV1 = 7,
    JC_DIGITAL_METER = 10,
    JC_ANALOG_METER = 11,
    JC_BOARD_DIN_WATCH_REV0 = 0x1000,
} jc_board_type;

struct history {
    long val;
    struct history *next;
    struct history *prew;
};

typedef struct {
    unsigned char res;
    unsigned char orgin_res;
} semaphore_t;

typedef struct {
    unsigned char bAutoReset;
    unsigned char bIsSet;
} event_t;

typedef enum {
    fsm_rt_err     = -1,
    fsm_rt_cpl     = 0,
    fsm_rt_on_going = 1
} fsm_rt_t;

typedef struct {
    unsigned char bLocked;
} critical_sector_t;

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

typedef unsigned char uchar, u8, BYTE;
typedef unsigned short  u16, WORD;

enum {
    False = 0, True
};
enum {
    showright, showleft
};
enum {
    LCDsize8 = 8,
    LCDsize12 = 12,
    LCDsize16 = 16,
    LCDsize24 = 24,
    LCDsize32 = 32,
    LCDsize40 = 40,
    LCDsize48 = 48,
    LCDsize64 = 64
};
enum {
    S0, S1, S2, S3, Smax
};
typedef enum {//枚举数据类型除以2得到的整数刚好
    Reserved = 0,
    myBoolType = 1,
    myCharType = 6,
    myUcharType = 7,
    myIntType = 10,
    myUintType = 11,
    myLongType = 16,
    myUlongType = 17,
    myStringType = 14,
    myFileType = 30
} MyDataType;
typedef enum {
    EEPROM_ERR,
    ADC_ERR,
    CALIBRA_ERR,
    PARAM_ERR,
    PASSWORD_ERR,
    PASSWORD_OK,
    CALIBRA_OK,
    SDCARD_ERR,
    CHKSUM_ERR
} Display_Blink_Type;


typedef struct {
    uchar addr;
    uchar func;
    UI    base;
    UI    wlenWord;
    uchar wlenByte;
    UL    val;
    UI    crc;
} TagWritePack;

typedef struct {
    uchar addr;
    uchar func;
    UI    base;
    UI    nReadWord;
    UI    crc;
} TagReadPack;

typedef  enum {
    HCA_HUMI_ACQUIRED,
    HCA_HUMI_DELETE,
    HCA_HUMI_ADJUST,
    HCA_TEMP_ADJUST,
} sensor_command_type;

typedef  enum {
    Y_MAN_CIRCLE_POINT = 0,     //Y轴手动，记录点画直线加圆点
    Y_MAN_LINE_POINT = 1,       //Y轴手动，记录点画直线
    Y_AUTO_CIRCLE_POINT  = 2,   //Y轴自动，记录点画直线加圆点
    Y_AUTO_LINE_POINT = 3,      //Y轴自动，记录点画直线
} curve_mode_type;
typedef  enum {
    NO_CHKSUM = 0,
    CHKSUM = 1,
} chk_sum_type;
typedef  enum {
    BATT_IS_OK = 0,
    BATT_IS_BAD = 1,
} batt_state_type;
typedef  enum {
    NO_DATA_TREND,
    GENTLY_TREND,
    UP_TREND,
    DOWN_TREND,
} trend_state;
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

typedef struct {
    /* bellow is ram only param */
    UL pv; //0，TFT显示值
    UL av; //2，从ADS1232读出的值
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
#if USE_DAC_OUTPUT == 1
    UL dac_out[DAC_CHMAX][DAC_BUF_NUM];
#endif
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
    UL a_av[6];//16，校准时第一点输入值
    UL a_pv[6];//32，显示校准时第一点输入值
    /* curve param */
    UL curve_channel;     //曲线只显示一条曲线时，选择显示哪一个变量，本仪表也不用
    UL curve_dis_mode; //曲线显示模式
    UL graph_x; //40
    UL graph_y1; //42
    UL graph_y2; //42
    UL crc;//68
} TagParam;

typedef struct {
    long display_index;
    long lenv_value;
    long temp_value;
    long humi_value;
    long drew_value;
    long ppmv_value;
} TagRuntime;
typedef struct {
    u8 delay_state;
    u8 save_key;
    long delay_time;
} Timer_Delay_Type;

typedef struct {
    unsigned long magic1;
    unsigned long crc;
    unsigned long magic2;
    unsigned char rsa[];
} CRC_SHA_PTR;

typedef struct  {
    long long modulus;
    long long exponent;
} public_key_class;
/* this tag use for global gpio map
 * Fix me latter
 */

#define BUS_BITS_SUPPORT 1
typedef struct jc_gpio_map {
    uint32_t i2c_scl;
    uint32_t i2c_sda;
    uint32_t key1;
    uint32_t key2;
    uint32_t key3;
    uint32_t key4;
    uint32_t key5;
    uint32_t key6;
    uint32_t msc_cs;
    uint32_t msc_miso;
    uint32_t msc_mosi;
    uint32_t msc_sclk;
    uint32_t msc_detect;
    uint32_t in_charge;
    uint32_t chr_done;
    uint32_t vbat;
    uint32_t re_485;
    uint32_t lcd_pwm;
    uint32_t bl_en;
    uint32_t ads1232_clk;
    uint32_t ads1232_dio;
    uint32_t ads1232_pdn;
    uint32_t pwr_on;
    uint32_t beep;
    uint32_t lcd_rst;
    uint32_t lcd_cso;
    uint32_t lcd_cd;
    uint32_t lcd_clk;
    uint32_t lcd_sda;
    uint32_t lcd_wr0;
    uint32_t lcd_wr1;
    uint32_t charge_check;
    uint32_t usb_vbus;
    uint32_t out_led;
    uint32_t dac1_di;
    uint32_t dac1_sclk;
    uint32_t dac1_sync;
    uint32_t dac2_di;
    uint32_t dac2_sclk;
    uint32_t dac2_sync;
    uint32_t out_x1;
    uint32_t out_x2;
#if BUS_BITS_SUPPORT
    uint32_t bus_d0;
    uint32_t bus_len;
#endif
} TagGpio;

typedef struct {
    MyDataType dataType;
    UL      longData;
    uchar   dataTemp[8];
    int     Index;
    uchar   flashFlag;
    uchar   ERRFlag;
    uchar   calibraFlag;
    char    *stringList;
    char    *unitstringList;
    uchar   negativeFlag;
    uchar   readonlyFlag;
    uchar   passwordFlag;
    uchar   cursorIndicate;
    uchar   menu_dot_Flag;
} ModifyDataType;

typedef struct {
    FATFS fs;      //文件系统
    DIR dir;        //文件目录
    FILINFO fno; //文件信息
    FIL file;    //文件
    char *dirname;  //目录路径名
    char *filename;//文件名字
    char *browsedirname;  //目录路径名
    char *browsefilename;//文件名字
    FRESULT res;    //返回结果
    unsigned int br;
    unsigned int wr;
    FRESULT (*f_mount)(FATFS* fs,   const TCHAR* path, BYTE opt);
    FRESULT (*f_lseek)(FIL *fp, DWORD ofs); //选择文件的末尾，从末尾写入数据时用
    int (*sd_show_free)(uint32_t *freeKB, uint32_t *freeMB); //显示SD卡空余空间
    int (*flash_show_free)(uint32_t *freeKB, uint32_t *freeMB); //显示SD卡空余空间
    FRESULT (*f_write)(FIL *fp, const void *buff, UINT btw, UINT *bw); //写数据到文件中
    FRESULT (*f_open)(FIL *fp, const TCHAR *path, BYTE mode); //打开或创建文件
    int (*delete_afile)(TCHAR *fname);                    //删除文件或目录
    int (*creat_afile)(FIL *fp, TCHAR *fname);             //创建文件或文件夹
    FRESULT (*f_read)(FIL *fp, void *buff, UINT btr, UINT *br); //读取文件数据
    FRESULT (*f_close)(FIL *fp);
} MyFileType;

typedef struct {
    unsigned char retryflag ;
    unsigned char sendcount;
    unsigned short retrytime;
} XmodemType;
typedef struct {
    u8 LCD_GRAM[128][12];
    u8 curve_xy[25];//时间轴坐标转换后的数据
    UL curve_pv[CURVE_DATA_LOG_NUM][25];//时间轴坐标 转换前的数据
    UL curve_pv_min[CURVE_DATA_LOG_NUM];//时间轴坐标 转换前的数据
    UL curve_pv_max[CURVE_DATA_LOG_NUM];//时间轴坐标 转换前的数据
    u8 curve_num[CURVE_DATA_LOG_NUM];
    u8 gui_paint_mode;
    u8 curve_mode;
} Curve_Typde;
enum {
    paramDIS,
    paramADV,
    paramCNT,
    paramDP,
    paramAddr,
    paramBaud,
    paramSpeed,
    paramZero,
    paramTrig,//追踪
    paramFilter,
    paramFilterBank,
    paramLimit,
    paramSense,
    paramAV1,
    paramAV2,
    paramAV3,
    paramAV4,
    paramAV5,
    paramAV6,
    paramAV7,
    paramPV1,
    paramPV2,
    paramPV3,
    paramPV4,
    paramPV5,
    paramPV6,
    paramPV7,
    paramMax = 127
};

enum {
    KEY_UP = 0X02, KEY_ENTER = 0X04, KEY_DOWM = 0X10, KEY_MENU = 0X01,  KEY_RIGHT = 0X20, KEY_LEFT = 0X08,
    KEY_MENU_CONFIG = 0X05, KEYSOFTTOEXITMENU = 0x3F,
};

typedef enum {
    K1, K2, K3, K4
} TagKeyScanfState;

typedef enum {
    NoUse,
    WorkNormal,
    WorkEnterMenu,
    SelectSubMenu,
    WorkMenu,
    WorkMenuFixParam,
    EnterPassword,
    DirSelectMenu,
    DirActionMenu,
    FileSelectMenu,
    FileActionMenu,
    FileBrowseMenu
} TagWorkState;

typedef  void (*interrupt_isr)(void);

typedef critical_sector_t mutex_t;
typedef unsigned char  uchar;
#ifndef USE_UNIX_BUILD_ENV
typedef unsigned short uint;
#endif
typedef unsigned long  ulong;
typedef int  bool;
//typedef int ssize_t, bool;

#define NULL2 (0)
#define false (0)
#define true (!false)


#define Blank 10

#define ADSK CK
#define ADD0 DO

#define TooLess        0x03  //30ms
#define ShortKeyLimit  0x06 //60ms
#define LongKeyLimit   150 //1500ms

#define ConstTimeH 60110/256
#define ConstTimeL 60110%256

#define InIicParamMax 32
#define RxBufMax 224
/*modreader最大可连续读取24个寄存器，也就是12个long数据，可能他的参数比较少，
但是我们的参数有80多个long数据，为减少RAM的使用，设置最大可读取数量为32个字节*/
#define TxBufMax 32*4+5

#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN       0XBC40 //棕色
#define BRRED       0XFC07 //棕红色
#define GRAY        0X8430 //灰色
#define DARKBLUE         0X01CF //深蓝色
#define LIGHTBLUE        0X7D7C //浅蓝色  
#define GRAYBLUE         0X5458 //灰蓝色
//以上三色为PANEL的颜色
#define LIGHTGREEN      0X841F //浅绿色
//#define LIGHTGRAY     0XEF5B //浅灰色(PANNEL)
#define LGRAY           0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE       0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE          0X2B12 //浅棕蓝色(选择条目的反色)

struct tag_comm_ctrl_data {
    uchar rx[RxBufMax];
    uchar tx[TxBufMax];
    uchar rx_index;
    uchar rx_cnt;

    uchar tx_index;
    uchar tx_cnt;
};
struct tag_nfc_ctrl_data {
    int gPn532RxLen;
    unsigned char *gpPn532RxBuf;//compatiple for other
};

/****************************************
第一级菜单有String,Next,Son,无Value
Menu1->Menu2->Menu3->Menu4->Menu5

第二级菜单有String,Value,Son,无Next
****************************************/
typedef enum {
    LANGUAGE_ENGLISH,
    LANGUAGE_CHINESE,
    LANGUAGE_MAX,
} TagLang;
typedef enum {
    MENUDISABLE,            //菜单不使能
    MENUENABLE,             //菜单使能
    MENUENABLE_PASSWORD,    //菜单使能并且有密码保护
} TagUseEnum;
typedef enum {
    FACTORY_NO,            //不恢复出厂设置
    FACTORY_YES,           //确认要恢复出厂设置
} Factiry_Setting;
typedef enum {
    /*密码拥有权限管理，就是设置为操作员密码的菜单，使用更高级管理员的密码也可以操作，和电脑一样*/
    PASSWORD_NOUSE = 0,           //不使用密码
    PASSWORD_OPER = 32,           //操作员密码
    PASSWORD_ADMIN = 64,          //超级管理员密码
    PASSWORD_SYS = 128,          //系统管理员密码
} Password_Code;
typedef struct MENU {
//  int  (*do_menu)(int val);
    void (*Write2eeprom)(uint addr, long l);
    long (*ReadFromeeprom)(uint addr);
    uint   MenuID;
    uint*  DynamicMenuID;
    char * DisplayString[LANGUAGE_MAX];
    struct MENU ** root_menu;
    struct MENU *NextMenu;
    struct MENU *PrevMenu;
    struct MENU * SonMenu;
    long FactoryValue;
    long Min;
    long Max;
    Password_Code Password;
    MyDataType DataType;
    TagUseEnum  *MenuUse;
    char *ParaInList[5];//总共5种List 供选择, 13 in total
} TagMenu;


enum {
    SelectString = 0, SelectNumber = 1, SelectBlack = 2, SelectEditor = 3
};

enum transfertype {
    uart_fd = 0,
    usb_fd = 1
};
enum {
    DATE_TIME_NO_UPDATE,
    UPDATE_HH_MM_SS,
    UPDATE_DATE_AND_TIME,
    UPDATE_REPAT_2,
    UPDATE_REPAT_3,
    UPDATE_REPAT_4,
    UPDATE_REPAT_5
};
typedef enum {
    Normal,
    PowerdownOut1k,
    PowerdownOut100k,
    PowerdownHighZ,
} DacModeType;

#define UART_SEND_BYTE(n,d) \
{\
    USART_SendData(n,d);\
    while(false==USART_GetFlagStatus(n, USART_FLAG_TC));\
}

#define BufferSelect(n) do{WhichBuffer=n;}while(0)

long abs_delta(long a, long b);
void Hx711_Init( void );
void Ex_Interrrupt_Init( void );
void Time0Init( void );
void SmgPrint( long, uchar);
void EditorPrint( long);
void ADCConvert( void );
void InitParam(TagParam *);
void SolveKey(void);
void BeepNoSync( unsigned char t);
// void DoWorkState( void );
void LoadALong2Vim( long );

void IICWritePara(uint addr, long);
long IICReadPara(uint addr);
void InitParaFromIICeeprom(TagParam *p);
void CheckMenu( void );
void UartInit( void );
void SendChar(unsigned char);
void DoModbus(TagParam *p, struct tag_comm_ctrl_data * ctrl, jc_comm_type type);
void DoNfc(struct tag_nfc_ctrl_data * ctrl);
void do_rx_comm(unsigned char);
void do_tx_comm(void);
void do_rx_comm2(unsigned char);
void do_tx_comm2(void);

bool get_event(event_t *ptEvent);
void set_event(event_t *ptEvent);
void clr_event(event_t *ptEvent);
void init_event(event_t *ptEvent, bool, bool);
void Timer_10ms_ISR(void);
int32_t get_dewpoint_scale(void);
void set_uart_idle_state(void);
void gpio_as_adc_init(void);
void beep_warn(uint8_t n);
fsm_rt_t check_vbatt_low(uint32_t);

void Init_UART1(void);
void Init_UART2(void);
void Init_Nfc(struct tag_comm_ctrl_data *);
void set_nfc_receive_point(unsigned char *pBuf);
void register_irq(int num, interrupt_isr fn);
bool RegisterADC(void *p);
bool flush_flash(void);
int  system_init(void);
void AdConvert(TagParam *p);
void init_adc(TagParam *p);
fsm_rt_t do_calibration(TagParam *p);
void  save_calib_result(TagParam *p);
TagParam *rt_get_param_addr(void);

void lcd_show_pmodifydata(const TagMenu *curser);
void dac8551_GPIO_Init ( void );
void WriteDataToDAC1(u16 wdata, DacModeType mode );
void WriteDataToDAC2(u16 wdata, DacModeType mode );
u16 dac_scale(TagParam *p, dac_channel_type dac_channel);

void LCD_Fill_new(u16 sx, u16 sy, u16 ex, u16 ey, u16 color);

uint crc16(uchar *puchMsg, uint usDataLen);
void log_data(TagRuntime *p);
fsm_rt_t copy_to_sdcard(void);
void update_logtime(unsigned long time);
int sd_file_init(uint32_t det);
int flash_file_init(void);
int sd_filesystem(void);
void sendafile(uint8_t transfermode);
void KeyandBeep_Init(void);
u8 Key_Scan(void);
void pwr_on(void);
void pwr_off(void);
void LCD_Backlight_Control(u8 pwm_dutycycle_select);
void update_curvetime(void);
long dewpoint_to_ppm_scale(long drew, uint32_t scale);
fsm_rt_t dewpoint_work(long *val, uint32_t scale);
void board_id_init(void);
void board_config_init(void);
void beep_deinit(void);
uint32_t gpio_get_value(uint32_t gpio);
void gpio_set_value(uint32_t gpio, int val);
void gpio_port_direction_input(uint32_t gpio, uint32_t state);
void gpio_port_direction_output(uint32_t gpio, uint32_t init_state, uint32_t mode);
void set_runtime_param_len(int len);
int  get_runtime_param_len(void);
void set_runtime_display_index(int index);
int get_runtime_display_index(void);
uint32_t read_board_id(void);
void do_send_request_sensor_data(struct tag_comm_ctrl_data *ctrl);
void do_parser_sensor_data(struct tag_comm_ctrl_data *ctrl, TagRuntime * pruntime);
uint32_t get_meter_type(void);
u16 bin_get_tnum(char *path);
void lcd_show_filename(int index, int filenum);
char* select_filename_return(int index);
void lcd_show_fileaction(int index);
u16 dir_get_tnum(char *path);
void lcd_show_dirname(int index, int filenum);
char* select_dirname_return(int index);
void lcd_show_diraction(int index);
u8 auto_create_dir_and_file(char *dirname, char *filename, u8 create_again);
void auto_create_dir_file(void);
FRESULT f_deldir(const TCHAR *path);
char* browse_filename_return(char *pfilename);
void lcd_show_browsefile_init(int* index, int browsefilenum);
void lcd_show_browsefile(int* index, int browsefilenum);
void time3_pwmdeinit(void);
TagRuntime* get_runtime_value(void);
void init_code_verify(void);
long get_image_size(void);
void menu_init(void);
FRESULT Generate_File_Structure(char *path);
fsm_rt_t task_usb(void);
fsm_rt_t ymodem_loop(void);
void hid_send_usb_data(uint8_t *str, uint16_t len);
void IWDG_Feed(void);
u8 get_detectdew_result(void);
void set_detectdew_result(u8 value);
void Display_Data_Trend(u16 x, u16 y, u8 size, trend_state datatrend, u8 mode);
long get_Read_Humi_Acquire_Value(void);
u8 Do_HCA_Command(sensor_command_type command, int setvalue);
void PC_communication_callback(long index, UL setvalue);
void set_pending_event(long event);
char * get_software_version(void);
char * get_serial_number(void);
char * get_device_plus_sensor_name(void);

extern ModifyDataType ModifyMenuData;
extern uchar WhichBuffer;
extern long  Temp;
extern char  New_AV_FLAG;
extern char  NegativeFlag;
extern char  CLEAR_FLAG;
extern char  UPDATE_FLASH_FLAG;
extern char  TIME_10S_FGAS_FLAG;
extern char  CommFlag, HidCommFlag;
extern char  ParserFlag;
extern uint  Tick;
extern uint  time_100ms;
extern uint  time_500ms;
extern uint  time_for_dewpoint;
extern uint  time_for_skip_100ms;
extern uint  flash_eeprom_delay;
extern uint  KeyPadTimeOut;
extern long  FixParam;
extern uchar caclib_index;
extern u8 dispaly_pwr_state;
extern u8 usb_displaystate;
extern  u8 sensor_err_count;
extern u8 serial_number[11 - 2];

//extern long InIicParam[];
extern long DisplayValue;
extern long FilterOut;

extern char TranceEnalbe;

extern uchar BeepCount;

extern const uchar auchCRCLo[];
extern const uchar auchCRCHi[];

extern unsigned char PF_IDR;
extern int drifting_delay;
extern u8 pwr_display;
extern unsigned long TimeInterval;
extern u8 update_to_curve;
extern MyFileType  myfile;
extern XmodemType xmodemdata;

extern TagLang lang;
extern uchar key_tone;
extern TagKeyScanfState KeyScanfState;
extern TagWorkState     WorkState;
extern TagWorkState     OldWorkState;
extern const TagMenu * Curser;
extern Curve_Typde Curve;
extern TagGpio jc_board_gpio_pin;
extern u8 Create_Again_Flag;
extern u8 Create_Again_Log_Flag;
extern char browsefilename[40];
extern int browsefile_rsize;  //浏览文件记录总条数
extern int browsefile_rcount;//浏览文件当前记录数
extern u8 sub_param_index;
//extern DMA_HandleTypeDef    hdma_adcx;
//extern UART_HandleTypeDef   husart1;
//extern DMA_HandleTypeDef    hdma_usart1_rx;
//extern UART_HandleTypeDef  husart2;
//extern TIM_HandleTypeDef TIM4_Handler;      //定时器句柄
extern char probe_type[7];
extern char probe_version[7];
extern char probe_serial_num[11];
extern char probe_name[11];
extern u8 alarm_beep;

extern char CommDAC1calibrationFlag;
extern char CommDAC2calibrationFlag;
extern uint CommDAC1calibrationTime;
extern uint CommDAC2calibrationTime;


extern  const TagMenu  Menu1, Menu2, Menu3, Menu4, Menu5, Menu6, Menu7, Menu8, Menu9, Menu10, Menu11;
extern  const TagMenu  Menu100, Menu101, Menu102, Menu103, Menu104, Menu105, Menu106, Menu107, Menu108, Menu109, Menu110;
extern  const TagMenu  Menu200, Menu201, Menu202, Menu203, Menu204, Menu205, Menu206, Menu207, Menu208, Menu209, Menu210, Menu211, Menu212, Menu213, Menu214;
extern  const TagMenu  Menu300, Menu301, Menu302, Menu303, Menu304, Menu305, Menu306, Menu307, Menu308, Menu309, Menu310,
        Menu311, Menu312;
extern  const TagMenu  Menu400, Menu401, Menu402, Menu403, Menu404;
extern  const TagMenu  Menu500, Menu501, Menu502, Menu503, Menu504, Menu505, Menu506, Menu507, Menu508, Menu509, Menu510, Menu511, Menu512, Menu513, Menu514;
extern  const TagMenu  Menu600, Menu601, Menu602, Menu603, Menu604, Menu605, Menu606, Menu607, Menu608, Menu609, Menu610, Menu611, Menu612, Menu613;
extern  const TagMenu  Menu700, Menu701, Menu702, Menu703, Menu704, Menu705, Menu706, Menu707, Menu708, Menu709, Menu710, Menu711, Menu712, Menu713;
extern  const TagMenu  Menu800, Menu801, Menu802, Menu803, Menu804, Menu805, Menu806, Menu807, Menu808, Menu809, Menu810, Menu811, Menu812, Menu813;
extern  const TagMenu  Menu900, Menu901, Menu902, Menu903, Menu904, Menu905, Menu906, Menu907, Menu908, Menu909, Menu910, Menu911, Menu912, Menu913;
extern  const TagMenu  Menu1000, Menu1001, Menu1002, Menu1003, Menu1004, Menu1005, Menu1006, Menu1007, Menu1008, Menu1009,  Menu1010;
extern  const TagMenu  Menu1100, Menu1101, Menu1102, Menu1103, Menu1104, Menu1105, Menu1106, Menu1107, Menu1108, Menu1109,  Menu1110, Menu1110,  Menu1111;

#include "platform_device.h"

#endif

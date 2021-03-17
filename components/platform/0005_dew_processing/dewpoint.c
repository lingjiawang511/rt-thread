#ifndef ENABLE_DEW_UNIT_TEST
#include <rthw.h>
#include <rtthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "include/HeadType.h"
#include "include/Filter.h"
#include "include/sensor.h"
#else
#include "../include/dewpoi_unit_test.h"
#endif

uint32_t caculate_humi_from_temp_dew(long temp_value, long dew_value, uint32_t scale);
#define RSP_MAX_TEMP  (9999999)
#define RSP_MAX_HUMI (99999)
#define RSP_MAX_DEW  (999999)
#define RSP_MAX_PPM  (9999999)
#define RSP_MIN_TEMP (-9999)
#define RSP_MIN_HUMI (-9999)
#define RSP_MIN_DEW  (-99999)
#define RSP_MIN_PPM  (0)

#define RSP_MAX_DP_WATCH_TEMP (99999)
#define RSP_MAX_DP_WATCH_HUMI (9999)
#define RSP_MAX_DP_WATCH_DEW  (2000)
#define RSP_MAX_DP_WATCH_PPM  (999999)
#define RSP_MIN_DP_WATCH_TEMP (-99999)
#define RSP_MIN_DP_WATCH_HUMI (0)
#define RSP_MIN_DP_WATCH_DEW  (-8000)
#define RSP_MIN_DP_WATCH_PPM  (0)
u8 sonsor_use_chksum = CHKSUM;
u8 sensor_err_count = 0;
typedef struct {
    trend_state temp_trend;
    trend_state humi_trend;
    trend_state dewpoint_trend;
    trend_state ppmv_trend;
} data_trend_type;
data_trend_type probe_data_trend;
extern void curve_data_dispose(long dat, uint8_t enable, uint8_t log_channal);
trend_state get_probe_data_trend(const char *getchar);
trend_state calculate_ppmv_data_trend(long ppmvdata);
char probe_type[7] = "xxxxxx\0";//客户传感器读出020，显示CRP1
char probe_version[7] = "xxxxxx\0";
char probe_serial_num[11] = "xxxxxxxxxx\0";
char probe_name[11] = "xxxxxxxxxx\0";

typedef struct {
    float dewpoint;
    float dewpointF;
    float water_press;
    float ppmv;
    float RH;
    float ppm_mass;
} d2p;

typedef struct {
    float A;
    float M;
    float Tn;
} d2ppm;

static const d2p dewpoint2ppm[] = {
    { -102,  -152, 0.0000065, 0.00855, 0.000035, 0.0053},
    { -100,  -148, 0.0000099, 0.0130, 0.000053, 0.0081},
    { -98, -144, 0.000015, 0.0197, 0.000080, 0.012 },
    { -96, -141, 0.000022, 0.0289, 0.00012,  0.018 },
    { -94, -137, 0.000033, 0.0434, 0.00018,  0.027 },
    { -92, -134, 0.000048, 0.0632, 0.00026,  0.039 },
    { -90, -130, 0.000070, 0.0921, 0.00037,  0.057 },
    { -88, -126, 0.00010, 0.132, 0.00054,  0.082 },
    { -86, -123, 0.00014, 0.184, 0.00075,  0.11 },
    { -84, -119, 0.00020, 0.263, 0.00107,  0.16 },
    { -82, -116, 0.00029, 0.382, 0.00155,  0.24 },
    { -80, -112, 0.00040, 0.526, 0.00214,  0.33 },
    { -78, -108, 0.00056, 0.737, 0.00300,  0.46 },
    { -76, -105, 0.00077, 1.01, 0.00410,  0.63 },
    { -74, -101, 0.00105, 1.38, 0.00559,  0.86 },
    { -72, -98, 0.00143, 1.88, 0.00762,  1.17 },
    { -70, -94, 0.00194, 2.55, 0.0104,   1.58 },
    { -68, -90, 0.00261, 3.43, 0.0140,   2.13 },
    { -66, -87, 0.00349, 4.59, 0.0187,   2.84 },
    { -64, -83, 0.00464, 6.11, 0.0248,   3.79 },
    { -62, -80, 0.00614, 8.08, 0.0328,   5.01 },
    { -60, -76, 0.00808, 10.6, 0.0430,   6.59 },
    { -58, -72, 0.0106, 13.9, 0.0565,   8.63 },
    { -56, -69, 0.0138, 18.2, 0.0735,   11.3 },
    { -54, -65, 0.0178, 23.4, 0.0948,   14.5 },
    { -52, -62, 0.0230, 30.3, 0.123,   18.8 },
    { -50, -58, 0.0295, 38.8, 0.157,   24.1 },
    { -48, -54, 0.0378, 49.7, 0.202,   30.9 },
    { -46, -51, 0.0481, 63.3, 0.257,   39.3 },
    { -44, -47, 0.0609, 80.0, 0.325,   49.7 },
    { -42, -44, 0.0768, 101, 0.410,   62.7 },
    { -40, -40, 0.0966, 127, 0.516,   78.9 },
    { -38, -36, 0.1209, 159, 0.644,   98.6 },
    { -36, -33, 0.1507, 198, 0.804,   122.9},
    { -34, -29, 0.1873, 246, 1.00,   152  },
    { -32, -26, 0.2318, 305, 1.24,   189  },
    { -30, -22, 0.2859, 376, 1.52,   234  },
    { -28, -18, 0.351, 462, 1.88,   287  },
    { -26, -15, 0.430, 566, 2.30,   351  },
    { -24, -11, 0.526, 692, 2.81,   430  },
    { -22, -8, 0.640, 842, 3.41,   523  },
    { -20, -4, 0.776, 1020, 4.13,   633  },
    { -18,  0, 0.939, 1240, 5.00,   770  },
    { -16,  3, 1.132, 1490, 6.03,   925  },
    { -14,  7, 1.361, 1790, 7.25,   1110 },
    { -12,  10, 1.632, 2150, 8.69,   1335 },
    { -10,  14, 1.950, 2570, 10.4,   1596 },
    { -8,  18, 2.326, 3060, 12.4,   1900 },
    { -6,  21, 2.765, 3640, 14.7,   2260 },
    { -4,  25, 3.280, 4320, 17.5,   2680 },
    { -2,  28, 3.880, 5100, 20.7,   3170 },
    { 0,  32, 4.579, 6020, 24.4,   3640 },
    { 2,  36, 5.294, 6970, 28.2,   4330 },
    { 4,  39, 6.101, 8030, 32.5,   4990 },
    { 6,  43, 7.013, 9230, 37.4,   5730 },
    { 8,  46, 8.045, 10590, 42.9,   6580 },
    { 10,  50, 9.209, 12120, 49.1,   7530 },
    { 12,  54, 10.52, 13840, 56.1,   8600 },
    { 14,  57, 11.99, 15780, 63.9,   9800 },
    { 16,  61, 13.63, 17930, 72.6,   11140},
    { 18,  64, 15.48, 20370, 82.5,   12650},
    { 20,  68, 17.54, 23080, 93.5,   14330}
};

#ifndef ENABLE_DEW_UNIT_TEST
#include "include/fastDewPoint.h"
#else
#include "../include/fastDewPoint.h"
#endif

typedef struct {
    char *request;
    char *respond;
} tag_sensor_data;

static const tag_sensor_data sensor_data = {
    {"{F99RDD}\r"},
    {"{F00rdd 001; 56.84;%rh;000;=; 27.41;癈;000;=;Dp; 18.10;癈;000;=;001;V3.2-1;0020107461;HC2         ;000;\n"}
};
long Read_Humi_Acquire_Value(struct tag_comm_ctrl_data *ctrl);
uint8_t do_sensor_HCA_command(struct tag_comm_ctrl_data *ctrl, sensor_command_type command, int setvalue);
int get_dew2ppm_const_index(long val)
{
    if (val < 0) {
        return 6;
    }
    if (val < 350) {
        return (val / 50);
    }
    return 0;
}

#define tokelvin(tin) (tin + 273.15)
// #define pres_const    (1013.3)
#define tohpa(val,k)  (val*k)

typedef double UFLOAT;
#if defined(__GNUC__)
#define user_exp expf
#define user_pow powf
#define user_log log
#else
#define user_exp exp
#define user_pow pow
#define user_log log
#endif

UFLOAT fair(UFLOAT pw, UFLOAT ptot, UFLOAT tk)
{
    UFLOAT loga;

    loga = (0.00004186 - 0.000000287 * tk + \
            0.000000000691 * tk * tk - \
            5.63E-13 * tk * tk * tk) * (ptot - pw);

    return user_exp(loga);
}

UFLOAT getenha(UFLOAT pw, UFLOAT ptot, UFLOAT tk)
{
    UFLOAT fcalc;

    fcalc = 1.0;

    if (tk > 573.15) {
        return fcalc;
    }

    fcalc = fair(pw, ptot, tk);

    if (fcalc < 1) {
        fcalc = 1;
    }

    return fcalc;
}

static const  UFLOAT Ci[] = {
    -7.85951783,
        1.84408259,
        -11.7866497,
        22.6807411,
        -15.9618719,
        1.80122502
    };

UFLOAT Pww(UFLOAT tk, UFLOAT ptot)
{
    UFLOAT teff;
    UFLOAT apu;
    UFLOAT pwcalc;

    teff = 1 - tk / 647.096;

    apu = 647.096 / tk * (Ci[0] * teff + Ci[1] * user_pow(teff, 1.5) + Ci[2] * teff * teff * teff + Ci[3] * user_pow(teff, 3.5) + Ci[4] * teff * teff * teff * teff + Ci[5] * user_pow(teff, 7.5));

    pwcalc = user_exp(apu) * 220640.0;

    return pwcalc * getenha(pwcalc, ptot, tk);
}
static const  UFLOAT Ai[] = {
    -5674.5359,
        6.3925247,
        -9.677843 * 0.001,//-9.677843E-03
        0.00000062215701,
        2.0747825 * 0.000000001,  //2.0747825E-09
        -9.484024 * 0.0000000000001,//-9.484024E-13
        4.1635019
    };
UFLOAT Pwi(UFLOAT tk, UFLOAT ptot)
{
    //calculates water vapor saturation pressure over ice T(K)
    UFLOAT apu;
    UFLOAT pwcalc;
    UFLOAT pw;

    if (tk > 273.15) { //T>0癈, return Pww(T)
        pw = Pww(tk, ptot);
    } else {//return Pwi(T)
        apu = Ai[0] / tk + Ai[1] + (Ai[2] * tk) + (Ai[3] * tk * tk) + (Ai[4] * tk * tk * tk) + (Ai[5] * tk * tk * tk * tk) + (Ai[6] * user_log(tk));

        pwcalc = user_exp(apu) / 100.0;
        pw = pwcalc * getenha(pwcalc, ptot, tk);
    }
    return pw;
}

UFLOAT get_state8(UFLOAT rh, UFLOAT pws)
{
    UFLOAT pwout = rh * pws / 100;
    return pwout;
}
UFLOAT Td_find(UFLOAT Pwfind, UFLOAT ptot)
{
    //find dewpoint from water vapor pressure
    UFLOAT astart;
    UFLOAT tguess;
    UFLOAT incr;
    UFLOAT p1;
    UFLOAT der;

    astart = user_log(Pwfind / 6.1078) / user_log(10); //first approximation
    tguess = 237.3 / (7.5 / astart - 1) + 273.15;
    incr = 0.01;
//refine approximation by iteration
    do {
        p1 = Pww(tguess, ptot);
        der = (Pww(tguess + incr, ptot) - p1) / incr;
        tguess = tguess + (Pwfind - p1) / der;
    } while ( fabs(Pwfind - p1) > 0.0000001);

    return tguess;
}
UFLOAT Tf_find(UFLOAT Pwfind, UFLOAT ptot)
{
    //find frostpoint from water vapor pressure
    UFLOAT astart;
    UFLOAT tguess;
    UFLOAT incr;
    UFLOAT p1;
    UFLOAT der;

    astart = user_log(Pwfind / 6.1134) / user_log(10); //first approximation
    tguess = 273.47 / (9.7911 / astart - 1) + 273.15;
    incr = 0.01;
//refine approximation by iteration
    do {
        p1 = Pwi(tguess, ptot);
        der = (Pwi(tguess + incr, ptot) - p1) / incr;
        tguess = tguess + (Pwfind - p1) / der;
    } while (fabs(Pwfind - p1) > 0.0000001);

    return tguess;
}
UFLOAT fromkelvin(UFLOAT tin)
{
    u8 tunit;
    UFLOAT tout;
    tunit = 1;
    switch (tunit) {
        case 1 ://C
            tout = tin - 273.15;
            break;
        case 2 ://F
            tout = (tin - 273.15) * 9 / 5 + 32;
            break;
        case 3 ://Kcase "K":
            tout = tin * 1.0;
            break;
        default :
            tout = tin * 1.0 - 273.15;
    }

    return tout;
}
UFLOAT caculate_dewpoint(UFLOAT temp, UFLOAT humi)
{
    UFLOAT pres  =  0;
    UFLOAT pws   =  0;
    UFLOAT pwin  =  0;
    UFLOAT dewpoint = 0;

    temp = tokelvin(temp);
    /*ptotal使用bar为单位，1bar = 1000mbar,1mbar= 100Pa,菜单单位为百分之一Pa,使用百分之一的菜单单位主要是考虑今后
    可以在实验室的环境使用，这样菜单修改压强可以修改到300bar*/
    pres = tohpa(rt_get_param_addr()->ptotal.l, 0.01);
    pws  = Pww(temp, pres);
    pwin = tohpa(get_state8(humi, pws), 1.0);

    dewpoint = Td_find(pwin, pres);
    if (dewpoint > 273.15) {
        dewpoint = fromkelvin(dewpoint);
    } else {
        dewpoint = fromkelvin(Tf_find(pwin, pres));
    }
    return dewpoint;
}
long vaisala_caculate_dew(long temp, long humi, uint32_t scale)
{
    UFLOAT dewvalue = 0;
    UFLOAT itemp = temp / (UFLOAT)scale;
    UFLOAT ihumi = humi / (UFLOAT)scale;

    dewvalue = caculate_dewpoint(itemp, ihumi);

    /* one dot for PPMv, so we scale 10 */
    return dewvalue * 100;
}
UFLOAT caculate_ppmvol(UFLOAT temp, UFLOAT humi)
{
    UFLOAT pres  =  0;
    UFLOAT pws   =  0;
    UFLOAT pwin  =  0;
    UFLOAT ppmvol = 0;

    temp = tokelvin(temp);
    /*ptotal使用bar为单位，1bar = 1000mbar,1mbar= 100Pa,菜单单位为百分之一Pa,使用百分之一的菜单单位主要是考虑今后
    可以在实验室的环境使用，这样菜单修改压强可以修改到300bar*/
    pres = tohpa(rt_get_param_addr()->ptotal.l, 0.01);

    pws  = Pww(temp, pres);

    pwin = tohpa(get_state8(humi, pws), 1.0);

    ppmvol = pwin / (pres - pwin) * 1E6;

    return ppmvol;
}

long vaisala_dew2ppm(long temp, long humi, uint32_t scale)
{
    UFLOAT ppmvol = 0;
    UFLOAT itemp = temp / (UFLOAT)scale;
    UFLOAT ihumi = humi / (UFLOAT)scale;

    ppmvol = caculate_ppmvol(itemp, ihumi);

    /* one dot for PPMv, so we scale 10 */
    return ppmvol * 10;
}
uint8_t calculate_sensor_chksum(uint8_t *chkdata, uint16_t chklen)
{
    uint8_t chksum;
    uint16_t i, sum = 0;
    for (i = 0; i < chklen; i++) {
        sum += *chkdata;
        chkdata++;
    }
    sum = sum & 0X003F;
    chksum = sum + 0X0020;
    return (uint8_t)chksum;
}

#ifndef ENABLE_DEW_UNIT_TEST
void do_send_request_sensor_data(struct tag_comm_ctrl_data *ctrl)
{
    static uint8_t chksum = 0;
    uint8_t *to  = (uint8_t*)(&(ctrl->tx[0]));

    memcpy(to, sensor_data.request, 9);
    sonsor_use_chksum = !!(rt_get_param_addr()->probe_use_chksum.l);
    if (sonsor_use_chksum == CHKSUM) {
        chksum = calculate_sensor_chksum(&(ctrl->tx[0]), 7);
        if (ctrl->tx[7] != chksum) {
            ctrl->tx[7] = chksum;//chksum代替}，其他不变
        }
    }
    ctrl->tx_cnt   = 9;
    ctrl->tx_index = 0;
    //DEBUG HAL_UART_Transmit_IT((UART_HandleTypeDef *)&husart1, ctrl->tx, ctrl->tx_cnt);
}
#endif

long do_parser_data_long(const char *str)
{
    long sum = 0;
    int dot = 2;
    uint8_t negative = 0;
    for (int i = 0 ; i < 8; i++) {
        if (str[i] == ' ') {
            continue;
        } else if (str[i] == '.') {
            dot = i;
            continue;
        } else if (str[i] == '-') {
            negative = 1;
            continue;
        } else if (str[i] == ';') {
            dot = i - dot;
            break;
        }
        sum *= 10;
        sum += (str[i] - '0');
    }
    sum = negative ? (sum * (-1)) : sum;
    switch (dot) {
        case 2:
            return sum;
        case 1:
            return sum * 10;
    }
    return sum;
}

long do_parser_data_long_no_dot(const char *str, float scale)
{
    long sum = 0;
    uint8_t negative = 0;

    for (int i = 0 ; i < 8; i++) {
        if (str[i] == ' ') {
            continue;
        } else if (str[i] == '-') {
            negative = 1;
            continue;
        } else if ((str[i] == 't') || (str[i] == 'v') || (str[i] == 'd')) {
            break;
        } else if ((str[i] > '9') || (str[i] < '0')) {
            break;
        }
        sum *= 10;
        sum += (str[i] - '0');
    }
    sum = negative ? (sum * (-1)) : sum;
    sum = (long)((scale * sum));

    return sum;
}

#ifndef ENABLE_DEW_UNIT_TEST
void do_take_care_about_unit_and_copy_to_param(TagRuntime * pruntime)
{
    /**温湿度露点PPM参数需要放在主数组中，通讯需要读取，DAC变送输出也需要读取 **/
    if (rt_get_param_addr()->unit_name.l == 1) {
        pruntime->temp_value = (long)(pruntime->temp_value * 1.8 + 32 * get_dewpoint_scale());
        pruntime->drew_value = (long)(pruntime->drew_value * 1.8 + 32 * get_dewpoint_scale());
    }
    rt_get_param_addr()->temp_value.l = pruntime->temp_value;
    rt_get_param_addr()->humi_value.l = pruntime->humi_value;
    rt_get_param_addr()->drew_value.l = pruntime->drew_value;
    rt_get_param_addr()->ppmv_value.l = pruntime->ppmv_value;
}

void do_parser_sensor_hc4(struct tag_comm_ctrl_data *ctrl, TagRuntime * pruntime, uint16_t chksum)
{
    sensor_err_count = 0;
    const char *cursor1 =  strstr((const char *)ctrl->rx, "%rh");
    const char *cursor2 =  strstr(cursor1, "癈");
    /* need to skip cursor2*/
    const char *cursor3 =  strstr((cursor2 + 4), "癈");

    /* get humi value
     * strncpy(buf, , 7);
     */
    //DEBUG SensorOnlineFlag = 1;
    pruntime->humi_value = do_parser_data_long(cursor1 - 7);
    if (pruntime->humi_value > RSP_MAX_HUMI) {
        pruntime->humi_value = RSP_MAX_HUMI;
    } else if (pruntime->humi_value < RSP_MIN_HUMI) {
        pruntime->humi_value = RSP_MIN_HUMI;
    }
    probe_data_trend.humi_trend = get_probe_data_trend(cursor1 + 8);
    /* get temp value */
    pruntime->temp_value = do_parser_data_long(cursor2 - 7);
    if (pruntime->temp_value > RSP_MAX_TEMP) {
        pruntime->temp_value = RSP_MAX_TEMP;
    } else if (pruntime->temp_value < RSP_MIN_TEMP) {
        pruntime->temp_value =  RSP_MIN_TEMP;
    }
    probe_data_trend.temp_trend = get_probe_data_trend(cursor2 + 7);
    /* get drew value */
    pruntime->drew_value = do_parser_data_long(cursor3 - 7);
    if (pruntime->drew_value > RSP_MAX_DEW) {
        pruntime->drew_value =   RSP_MAX_DEW;
    } else if (pruntime->drew_value < RSP_MIN_DEW) {
        pruntime->drew_value =  RSP_MIN_DEW;
    }
    probe_data_trend.dewpoint_trend = get_probe_data_trend(cursor3 + 7);
    if (get_dewpoint_scale() != 100) { //小数点转换，不影响原来的计算，PPM固定显示一位小数点，因为显示宽度不够
        if (pruntime->humi_value < 0) {
            pruntime->humi_value = pruntime->humi_value * (-1);
            pruntime->humi_value = pruntime->humi_value * get_dewpoint_scale() / 100;
            pruntime->humi_value = pruntime->humi_value * (-1);
        } else {
            pruntime->humi_value = pruntime->humi_value * get_dewpoint_scale() / 100;
        }
        if (pruntime->temp_value < 0) {
            pruntime->temp_value = pruntime->temp_value * (-1);
            pruntime->temp_value = pruntime->temp_value * get_dewpoint_scale() / 100;
            pruntime->temp_value = pruntime->temp_value * (-1);
        } else {
            pruntime->temp_value = pruntime->temp_value * get_dewpoint_scale() / 100;
        }
        if (pruntime->drew_value < 0) {
            pruntime->drew_value = pruntime->drew_value * (-1);
            pruntime->drew_value = pruntime->drew_value * get_dewpoint_scale() / 100;
            pruntime->drew_value = pruntime->drew_value * (-1);
        } else {
            pruntime->drew_value = pruntime->drew_value * get_dewpoint_scale() / 100;
        }
    }
    /* cac ppmv value, ppmv only had one dot */
    pruntime->ppmv_value = vaisala_dew2ppm( pruntime->temp_value, \
                                            pruntime->humi_value, get_dewpoint_scale());
    if (pruntime->ppmv_value > RSP_MAX_PPM) {
        pruntime->ppmv_value =  RSP_MAX_PPM;
    }  else if (pruntime->ppmv_value < RSP_MIN_PPM) {
        pruntime->ppmv_value =  RSP_MIN_PPM;
    }
    probe_data_trend.ppmv_trend = calculate_ppmv_data_trend(pruntime->ppmv_value);
    strncpy((char *)probe_version, (char *)(cursor3 + 13), 6);
    strncpy((char *)probe_serial_num, (char *)(cursor3 + 20), 10);
    strncpy((char *)probe_name, (char *)(cursor3 + 31), 10);

    do_take_care_about_unit_and_copy_to_param(pruntime);
}

#undef SENSOR_PARSER_EMULATION
int do_parser_sensor_msp(struct tag_comm_ctrl_data *ctrl, TagRuntime * pruntime, uint16_t chksum)
{
#ifdef SENSOR_PARSER_EMULATION
    const char * recv =  "t15636v343d10100t15636v343d10100\n";
    const char *cursor1 =  strchr((const char *)recv, 't');
#else
    /* need to skip cursor1 which is sensor's av value */
    char *cursor1 =  strchr((char *)ctrl->rx, 't');
#endif

    sensor_err_count = 0;
    char *cursor2 =  strchr(cursor1, 'v');
    char *cursor3 =  strchr((cursor2 + 1), 'd');

    /* set online flag */
    //DEBUG SensorOnlineFlag = 1;

    /* get temp value */
    pruntime->temp_value = do_parser_data_long_no_dot(cursor2 + 1, 10.0);
    if (pruntime->temp_value > RSP_MAX_TEMP) {
        pruntime->temp_value = RSP_MAX_TEMP;
    } else if (pruntime->temp_value < RSP_MIN_TEMP) {
        pruntime->temp_value =  RSP_MIN_TEMP;
    }

    /* get dewpoint value */
    pruntime->drew_value = do_parser_data_long_no_dot(cursor3 + 1, 0.1);
    if (pruntime->temp_value > RSP_MAX_TEMP) {
        pruntime->temp_value = RSP_MAX_TEMP;
    } else if (pruntime->temp_value < RSP_MIN_TEMP) {
        pruntime->temp_value =  RSP_MIN_TEMP;
    }

    /* cac humi value from temp plus dewpoint value */
    pruntime->humi_value = caculate_humi_from_temp_dew(pruntime->temp_value, pruntime->drew_value, get_dewpoint_scale());
    /* cac ppmv value, ppmv only had one dot */
    pruntime->ppmv_value = vaisala_dew2ppm( pruntime->temp_value, \
                                            pruntime->humi_value, get_dewpoint_scale());
    if (pruntime->ppmv_value > RSP_MAX_PPM) {
        pruntime->ppmv_value =  RSP_MAX_PPM;
    }  else if (pruntime->ppmv_value < RSP_MIN_PPM) {
        pruntime->ppmv_value =  RSP_MIN_PPM;
    }

    do_take_care_about_unit_and_copy_to_param(pruntime);
    return 0;
}

UFLOAT caculate_ppm_from_dew(long dew_value, uint32_t scale);
static TagRuntime runtime = {0};
uint32_t do_processing_srt20x68(int32_t in_temp, int32_t in_dew)
{
    TagRuntime * pruntime = &runtime;

    /* convert the value to take care about unit */
    int32_t temp = in_temp * get_dewpoint_scale() / 100;
    int32_t dew  = in_dew * get_dewpoint_scale() / 100;

    pruntime->temp_value =  temp;
    pruntime->drew_value =  dew;

    /* cac humi value from temp plus dewpoint value */
    pruntime->humi_value = caculate_humi_from_temp_dew(temp, dew, get_dewpoint_scale());
    /* cac ppmv value, ppmv only had one dot */
    UFLOAT ppm =  caculate_ppm_from_dew(dew, get_dewpoint_scale());
    if (dew > (-8000)) {
        pruntime->ppmv_value = ppm / 100000.0;
    } else {
        pruntime->ppmv_value = ppm / 100000.0;
        /* if change unit, use this value: ppm / 100.0 */
    }
    if (pruntime->ppmv_value > RSP_MAX_PPM) {
        pruntime->ppmv_value =  RSP_MAX_PPM;
    }  else if (pruntime->ppmv_value < RSP_MIN_PPM) {
        pruntime->ppmv_value =  RSP_MIN_PPM;
    }

    rt_kprintf("temp=%d humi=%d dew=%d ppm=%d\n", temp, pruntime->humi_value, dew, pruntime->ppmv_value);

    do_take_care_about_unit_and_copy_to_param(pruntime);
}

void do_parser_sensor_data(struct tag_comm_ctrl_data *ctrl, TagRuntime * pruntime)
{
    static uint8_t chksum_errcount = 0;
    uint16_t chksum = 0;

    if ((2 > ctrl->rx_cnt) || (512 <= ctrl->rx_cnt)) {
        /* overflow found */
        return;
    }

    if (target_is_watch_meter()) {
        do_parser_sensor_msp(ctrl, pruntime, chksum);
        return;
    }
    if (sonsor_use_chksum == CHKSUM) {
        chksum = calculate_sensor_chksum(&(ctrl->rx[0]), ctrl->rx_cnt - 2);
    }
    if ((chksum == ctrl->rx[ctrl->rx_cnt - 2]) || ((sonsor_use_chksum == NO_CHKSUM))) {
        do_parser_sensor_hc4(ctrl, pruntime, chksum);
    } else {  //3次校验错误显示错误码然后清屏重新显示
        chksum_errcount++;
        if (chksum_errcount > 3) {
            //Gui_Lcd_Clear(BLACK);
            //Gui_display_error(16, 4, CHKSUM_ERR, LCDsize24, 0);
            //Gui_Lcd_Clear(BLACK);
            chksum_errcount = 0;
        }
    }
}
#endif

const uint8_t USE_FROSTPOINT = 1;
uint32_t caculate_humi_from_temp_dew(long temp_value, long dew_value, uint32_t scale)
{
    UFLOAT pres =  0;
    UFLOAT pws  =  0;
    UFLOAT pwe  =  0;
    UFLOAT sc = scale;

    UFLOAT itemp = temp_value / sc;
    UFLOAT idew =  dew_value / sc;

    pres = tohpa(rt_get_param_addr()->ptotal.l, 0.01);

    if (USE_FROSTPOINT) {
        pws  = Pwi(tokelvin(itemp), pres);
        pwe  = Pwi(tokelvin(idew), pres);
    } else {
        pws  = Pww(tokelvin(itemp), pres);
        pwe  = Pww(tokelvin(idew), pres);
    }

    sc = 100 * scale * pwe / pws;
    return (uint32_t)(sc);
}

UFLOAT caculate_ppm_from_dew(long dew_value, uint32_t scale)
{
    UFLOAT fed  =  1.000401985493053;
    //UFLOAT fed  =  1.00;
    UFLOAT pres =  0;
    UFLOAT pws  =  0;
    UFLOAT sc = scale;

    UFLOAT idew =  dew_value / sc;

    pres = tohpa(rt_get_param_addr()->ptotal.l, 0.01);

    pws  = Pwi(tokelvin(idew), pres);

    sc = 1000000.0 * scale * 1000;
    sc = (sc * fed * pws) / ((pres) - (pws * fed));
    return (sc);
}

long dewpoint_to_ppm_scale(long dew, uint32_t scale)
{
    int i = 0;
    float y;
    float av;

    av = dew;
    av /= scale;

    if (av < -100) {
        return (0.0130 * scale);
    } else if (av >= 20) {
        return  (23080 * scale);
    }
    for (i = 0; i < (sizeof(dewpoint2ppm) / sizeof(dewpoint2ppm[0]) - 1); i++) {
        if (av == dewpoint2ppm[i].dewpoint) {
            return dewpoint2ppm[i].ppmv;
        } else if (av < dewpoint2ppm[i + 1].dewpoint) {
            break;
        }
    }

    y =  ((dewpoint2ppm[i + 1].ppmv - dewpoint2ppm[i].ppmv)) * (av - (dewpoint2ppm[i].dewpoint)) \
         / (dewpoint2ppm[i + 1].dewpoint - dewpoint2ppm[i].dewpoint) \
         + dewpoint2ppm[i].ppmv;
    y = y * scale;
    return y;
}

static UFLOAT dewpoint11, dewpoint22, deltadewpoint;
static long  origin_dewpoint = 0;
static uint8_t redled = 0;


uint8_t read_dewpoint_status(void)
{
    if (redled) {
        return true;
    }
    return false;
}

long cac_graph(long val, uint32_t scale)
{
    UFLOAT dewpoint  = 0;
    UFLOAT dewpoint1 = 0;

    dewpoint1 = val;
    dewpoint1 = dewpoint1 / scale;

    if (dewpoint1 >= -13.9) {
        dewpoint = dewpoint1 * 1.0;
    } else if (dewpoint1 >= -38.6) {
        dewpoint = dewpoint1 * 1.0534 + 0.4619; /* "曲线关系式"*/
    } else {
        dewpoint = 1.6084 * dewpoint1 + 22.387; /* "曲线关系式"*/
    }

    if (dewpoint >= 20.0) {
        dewpoint = 20.0;
    }
    if (dewpoint <= (-100.0)) {
        dewpoint = (-100.0);
    }

    dewpoint *= scale ;
    return  dewpoint;
}

/*
 * get skip time from Jiawang on 2018-04-14 by phone call: 8 seconds.
 * that is to say, for 100ms softtimer, use time = 8000/100 = 80,
 * this param is in param, So need set rt_get_param_addr()->dewpoint_skip_time.l to
 * 80 and it will working fine.
 */
#ifndef ENABLE_DEW_UNIT_TEST
uint16_t time_for_dewpoint = 0;
fsm_rt_t skip_time(void)
{
    static enum {
        STATE_START,
        STATE_DELAY,
        STATE_CPL
    } s_state = STATE_START;

    switch (s_state) {
        case STATE_START: /* skip time unit is 100ms per */
            time_for_dewpoint = rt_get_param_addr()->dewpoint_skip_time.l;
            s_state++;
        case STATE_DELAY:
            if (!time_for_dewpoint) {
                s_state = STATE_CPL;
            }
            break;
        case  STATE_CPL:
            s_state = STATE_START;
            return fsm_rt_cpl;
        default:
            break;
    }

    return fsm_rt_on_going;
}

long FastDetectDewpoint(long dp, long scale);
fsm_rt_t dewpoint_work(long * val, uint32_t scale)
{
    static enum {
        STATE_START,
        STATE_GET_DEWPOINT11,
        STATE_CAC_GRAPH1,
        STATE_GET_DEWPOINT22,
        STATE_CPL,
    } s_state = STATE_START;

    /* fast_dewpoint is a long hasn't dot */
    origin_dewpoint = *val;

    switch (s_state) {
        case STATE_START:
        case  STATE_GET_DEWPOINT11: /*"作为比较的第二个露点值"*/
            dewpoint11 = origin_dewpoint;
            /* note that dewpoint11 is real value that has dot */
            dewpoint11 /= scale;
            s_state = STATE_CAC_GRAPH1;
            break;
        case  STATE_CAC_GRAPH1:
            //*val = cac_graph(origin_dewpoint, scale);
            *val = FastDetectDewpoint(origin_dewpoint, scale);
            if (fsm_rt_cpl == skip_time()) {
                s_state = STATE_GET_DEWPOINT22;
            }
            break;
        case  STATE_GET_DEWPOINT22: /*"作为比较的第二个露点值"*/
            dewpoint22 = origin_dewpoint;
            dewpoint22 /= scale;
        case  STATE_CPL:
            deltadewpoint = dewpoint11 - dewpoint22;
            if ((deltadewpoint <= 0.1) && (deltadewpoint >= (-0.1))) {
                set_detectdew_result(1);
                //to do:  gpio_set_vaule();
                s_state = STATE_START;
                return fsm_rt_cpl;
            } else {
                // to do: gpio_set_vaule();
                set_detectdew_result(0);
            }
            s_state = STATE_START;
            break;
    }
    return fsm_rt_on_going;
}

uint8_t get_detectdew_result(void)
{
    return redled;
}
void set_detectdew_result(uint8_t value)
{
    if (value == 0) { //the hardware is opposite logic
        gpio_set_value(jc_board_gpio_pin.out_led, 1);
    } else {
        gpio_set_value(jc_board_gpio_pin.out_led, 0);
    }
    redled = value;
}
#endif

long binarySearch(const long* ListData, int ListLenth, long KeyData)
{
    int low = 0;
    int high = ListLenth - 1;
    long mid, startVal, endVal;
    while (low < high) { //because use mid+1,so low can not equal high
        mid = (low + high) / 2;
        startVal = ListData[mid];
        endVal = ListData[mid + 1];
        if (endVal < KeyData) {
            low = mid + 1;
        } else if (startVal > KeyData) {
            high = mid;
        } else if (endVal == KeyData) {
            return mid;
        } else {
            return mid;
        }
    }
    return -1;
}
//the dewpoint have 3 dot
long FastDetectDewpoint(long dp, long scale)
{
    double detectDew, fk;
    long startnum;
    long ret = 0;
    /* need set the dot point to same as data from header file */
    long standardDew = dp * 1000 / scale;
    /* check data range */
    if (standardDew <= FastDewTable[0]) {
        ret = FastDewTable[FASTDEW_GET_INDEX(1, 0)];
        goto find;
    } else if (standardDew >= FastDewTable[FASTDEW_GET_INDEX(0, FASTDEWLENTH) - 1]) {
        ret = FastDewTable[FASTDEW_GET_INDEX(1, FASTDEWLENTH) - 1];
        goto find;
    }

    /* search in data */
    startnum = binarySearch(&FastDewTable[0], FASTDEWLENTH, standardDew);
    if (startnum == -1) {
        /* nothing found */
        ret =  standardDew;
    } else if (startnum <= FASTDEWLENTH - 2) {
        /* caculate delta Y */
        fk = (FastDewTable[FASTDEW_GET_INDEX(1, startnum + 1)] -   \
              FastDewTable[FASTDEW_GET_INDEX(1, startnum)]);
        if (0 == fk) {
            /* mutiple x to one Y */
            ret = FastDewTable[FASTDEW_GET_INDEX(1, startnum)];
            goto find;
        }
        /* caculate K, k=(deltaY)/(deltaX)*/
        fk = fk /  ((FastDewTable[FASTDEW_GET_INDEX(0, startnum + 1)] - \
                     FastDewTable[FASTDEW_GET_INDEX(0, startnum)]));

        /* caculate new y=kx+b */
        detectDew = fk * (standardDew - FastDewTable[FASTDEW_GET_INDEX(0, startnum)]) + \
                    (FastDewTable[FASTDEW_GET_INDEX(1, startnum)]);
        ret =  detectDew;
    } else {
        /* this should not happend */
        while (1);
    }

find:
    return ret * scale / 1000;
}
trend_state get_probe_data_trend(const char *getchar)
{
    trend_state rettrend;
    switch (*getchar) {
        case '+':
            rettrend = UP_TREND;
            break;
        case '-':
            rettrend = DOWN_TREND;
            break;
        case '=':
            rettrend = GENTLY_TREND;
            break;
        case ' ':
            rettrend = NO_DATA_TREND;
            break;
        default:
            rettrend = NO_DATA_TREND;
            break;
    }
    return rettrend;
}
uint32_t simple_filter(uint32_t *val, int n)
{
    uint32_t sum;
    uint32_t min;
    uint32_t max;

    sum = min = max = val[0];

    for (int i = 1 ; i < n; i++) {
        sum += val[i];
        if (val[i] > max) {
            max = val[i];
        }
        if (val[i] < min) {
            min = val[i];
        }
    }
    sum = sum - min - max;
    sum /= (n - 2);

    return sum;
}
trend_state calculate_ppmv_data_trend(long ppmvdata)
{
    /**判断依据是连续三次大于FILTE_TREND就认为数据是上升的，连续三次小于FILTE_TREND
    就认为是数据是下降的，其他的都是平缓的，输入为0的时候属于没有数据趋势**/
#define FILTE_TREND  10
#define TREND_COUNT  3
#define FILTE_COUNT_MAX  5
    static uint32_t ppmv_history[FILTE_COUNT_MAX] = {0};
    static trend_state data_trend, old_trend = GENTLY_TREND;
    static long olddata1;
    static uint8_t up_count = 0, dowm_count = 0, gentlycount = 0, ppmv_index = 0;

    data_trend = old_trend;
    ppmv_history[ppmv_index] = ppmvdata;
    if (ppmv_index++ < FILTE_COUNT_MAX) {
        return data_trend;
    }
    ppmv_index = 0;
    if (ppmvdata == 0) {
        data_trend = NO_DATA_TREND;
        up_count = 0;
        dowm_count = 0;
        gentlycount = 0;
    } else if (ppmvdata > (olddata1 + FILTE_TREND)) {
        up_count++;
        if (up_count >= TREND_COUNT) {
            up_count = TREND_COUNT;
            data_trend = UP_TREND;
            dowm_count = 0;
            gentlycount = 0;
        }
    } else if ((ppmvdata + FILTE_TREND) < olddata1) {
        dowm_count++;
        if (dowm_count >= TREND_COUNT) {
            dowm_count = TREND_COUNT;
            data_trend = DOWN_TREND;
            up_count = 0;
            gentlycount = 0;
        }
    } else {
        gentlycount++;
        if (gentlycount >= TREND_COUNT) {
            gentlycount = TREND_COUNT;
            data_trend = GENTLY_TREND;
            up_count = 0;
            dowm_count = 0;
        }
    }

    olddata1 = simple_filter(ppmv_history, FILTE_COUNT_MAX);
    old_trend = data_trend;
    return data_trend;
}
trend_state get_temp_data_trend(void)
{
    if (rt_get_param_addr()->trend_funtion.l == 1) {
        return probe_data_trend.temp_trend;
    } else {
        return NO_DATA_TREND;
    }
}
trend_state get_humi_data_trend(void)
{

    if (rt_get_param_addr()->trend_funtion.l == 1) {
        return probe_data_trend.humi_trend;
    } else {
        return NO_DATA_TREND;
    }
}
trend_state get_dewpoint_data_trend(void)
{
    if (rt_get_param_addr()->trend_funtion.l == 1) {
        return probe_data_trend.dewpoint_trend;
    } else {
        return NO_DATA_TREND;
    }
}
trend_state get_ppmv_data_trend(void)
{
    if (rt_get_param_addr()->trend_funtion.l == 1) {
        return probe_data_trend.ppmv_trend;
    } else {
        return NO_DATA_TREND;
    }
}
void deinit_probe_data_trend(void)
{
    probe_data_trend.temp_trend = NO_DATA_TREND;
    probe_data_trend.humi_trend = NO_DATA_TREND;
    probe_data_trend.dewpoint_trend = NO_DATA_TREND;
    probe_data_trend.ppmv_trend = NO_DATA_TREND;
}

#ifndef ENABLE_DEW_UNIT_TEST
static void Send_Sensor_Command(struct tag_comm_ctrl_data *ctrl, const char* send_string, uint16_t send_cnt)
{
    uint8_t *to  = (uint8_t*)(&(ctrl->tx[0]));
    static uint8_t chksum = 0;

    memcpy(to, send_string, send_cnt);
    sonsor_use_chksum = !!(rt_get_param_addr()->probe_use_chksum.l);
    if (sonsor_use_chksum == CHKSUM) {
        chksum = calculate_sensor_chksum(&(ctrl->tx[0]), send_cnt - 2);
        if (ctrl->tx[send_cnt - 2] != chksum) {
            ctrl->tx[send_cnt - 2] = chksum; //chksum代替}，其他不变
        }
    }
    ctrl->tx_cnt   = send_cnt;
    ctrl->tx_index = 0;
    //DEBUG HAL_UART_Transmit_IT((UART_HandleTypeDef *)&husart1, ctrl->tx, ctrl->tx_cnt);
}
char ParserFlag;

long Read_Humi_Acquire_Value(struct tag_comm_ctrl_data *ctrl)
{
    const char* ERD_HUMI_Acquired = {"{F99ERD 0;1282;001;}\r"};
    char* pcursor;
    uint16_t readtimeout = 0;
    long retvalue;

    Send_Sensor_Command(ctrl, ERD_HUMI_Acquired, 21);
    //DEBUG ParserFlag = 0;
    readtimeout = 0;
    while (!ParserFlag) {
        rt_thread_sleep(5);
        readtimeout++;
        if (readtimeout >= 300) {
            break;
        }
    }
    if (ParserFlag) {
        if (ctrl->rx_cnt >= 13 ) {
            pcursor = (char *)&ctrl->rx[8];
            retvalue = (*pcursor++ - '0') * 100;
            retvalue += (*pcursor++ - '0') * 10;
            retvalue += (*pcursor - '0');
        }
    } else {
        retvalue = -1;
    }
    ParserFlag = 0;
    return retvalue;
}

uint8_t do_sensor_HCA_command(struct tag_comm_ctrl_data *ctrl, sensor_command_type command, int setvalue)
{
    const char* HCA_HUMI_Acquired = {"{F99HCA 0;1;0;62.56;}\r"};
    const char* HCA_HUMI_Delete = {"{F99HCA 0;1;3;;}\r"};
    const char* HCA_HUMI_Adjust = {"{F99HCA 0;1;1;;}\r"};
    const char* HCA_TEMP_Adjust = {"{F99HCA 0;2;1;29.16;}\r"};
    char send_buf[30];
    uint16_t len = 5;
    char* pcursor;
    uint16_t readtimeout = 0;
    uint8_t retvalue;

    //DEBUG Show_Str(4 * 6, 10, WHITE, BLUE, "Please Wait ", LCDsize8, 0);
    rt_thread_sleep(500);
    if ((command == HCA_HUMI_ACQUIRED) || (command == HCA_TEMP_ADJUST)) {
        if (command == HCA_HUMI_ACQUIRED) {
            strncpy((char*)send_buf, HCA_HUMI_Acquired, 14);
        } else {
            strncpy((char*)send_buf, HCA_TEMP_Adjust, 14);
        }
        if (setvalue > 9999) {
            len = sprintf((char*)(&send_buf[14]), "%3d.%02d", (setvalue / 100), (setvalue % 100));
        } else if (setvalue < 0) {
            setvalue = setvalue * (-1);
            if (setvalue > 999) {
                len = sprintf((char*)(&send_buf[14]), "-%2d.%02d", (setvalue / 100), (setvalue % 100));
            } else {
                len = sprintf((char*)(&send_buf[14]), "-%1d.%02d", (setvalue / 100), (setvalue % 100));
            }
        } else {
            len = sprintf((char*)(&send_buf[14]), "%2d.%02d", (setvalue / 100), (setvalue % 100));
        }
        strncpy((char*)(&send_buf[14 + len]), ";}\r", 3);
        Send_Sensor_Command(ctrl, send_buf, len + 17);
    } else if (command == HCA_HUMI_DELETE) {
        Send_Sensor_Command(ctrl, HCA_HUMI_Delete, 17);
    } else if (command == HCA_HUMI_ADJUST) {
        Send_Sensor_Command(ctrl, HCA_HUMI_Adjust, 17);
    }
    ParserFlag = 0;
    readtimeout = 0;
    while (!ParserFlag) {
        rt_thread_sleep(10);
        readtimeout++;
        if (readtimeout >= 600) {
            break;
        }
    }
    if (ParserFlag) {
        if (ctrl->rx_cnt >= 10 ) {
            pcursor = (char *)&ctrl->rx[8];
            if (strncmp((char*)pcursor, "OK", 2) == 0) {
                retvalue = 1;
            }
        }
    } else {
        retvalue = 0;
    }
    ParserFlag = 0;
    return retvalue;
}
#endif

#ifdef ENABLE_DEW_UNIT_TEST

#define TEST_DEWPOINT 0
#if TEST_DEWPOINT
static float test_input[121] = {0};
static float test_output[121] = {0};
void test_dewpoint(void)
{
    int j = 0;
    for (int i = -100; i < 20; i++) {
        test_output[j] = dewpoint_to_ppm(i);
        test_input[j] = dewpoint_to_ppm_scale(i + 0.01);
        j++;
    }
}
#endif

/*
 * This unit test is build on linux x64 using GCC
 * If you are use windows, compile with mingw is working fine.
 * Use this command to build: gcc -DENABLE_DEW_UNIT_TEST -DUSE_UNIX_BUILD_ENV
 *                                -o main platform/kernel/dewpoint.c -lm
 */
int main(int argc, char *argv[])
{
    int temp = 0;
    int dew  = 0;
    int val[2] = {0, 0};

    if (3 == argc) {
        val[0] = atoi(argv[1]);
        val[1] = atoi(argv[2]);
        printf("temp=%d,dew=%d\n", val[0], val[1]);
        printf("humi=%d\n", caculate_humi_from_temp_dew(val[0], val[1], 100));
    } else if (2 == argc) {
        if (0 == strcmp(argv[1], "cal_humi")) {
            /* 2 dot in data */
            for (dew = -8000; dew < 2000; dew += 11) {
                for (temp = dew + 200; temp < 2000; temp += 200) {
                    float humi =  caculate_humi_from_temp_dew(temp, dew, 100);
                    printf("%.2f,%.2f,%.2f\n", temp / 100.0, dew / 100.0, humi / 100.0);
                }
            }
        } else if (0 == strcmp(argv[1], "cal_ppm")) {
            /* 2 dot in data */
            for (dew = -12000; dew < 2000; dew += 100) {
                float ppm =  caculate_ppm_from_dew(dew, 100);
                if (dew > -8000) {
                    printf("%.2f,%.3f\n", (float)dew / 100.0, ppm / 100000.0);
                } else {
                    printf("%.2f,%.3f\n", (float)dew / 100.0, ppm / 100.0);
                }
            }
        }
    }
}
#endif

#define RSA_MB_POLL_LENGTH (40)

typedef struct {
    struct rt_mailbox mb;
    char mb_pool[RSA_MB_POLL_LENGTH];
} tag_mailbox;

static tag_mailbox mailbox;

struct sensor_struct {
    int32_t sensor_type;
    int32_t dew;
    int32_t temp;
};

static rt_uint8_t thread_dew_processing_handle_stack[1024];
struct rt_thread thread_dew_processing_handle;

struct rt_mailbox *get_dew_process_mailbox(void)
{
    return (&(mailbox.mb));
}

void thread_entry_dew_processing (void* parameter)
{
    struct sensor_struct *psrt;

    /* check ptotal */
    if ((rt_get_param_addr()->ptotal.l < (101330 / 5)) || (rt_get_param_addr()->ptotal.l > (5 * 101330))) {
        rt_get_param_addr()->ptotal.l  = 101330;
    }
    /* check dot number*/
    if ((rt_get_param_addr()->dot_num.l < 0) || (rt_get_param_addr()->dot_num.l > 2)) {
        rt_get_param_addr()->dot_num.l = 2;
    }
    rt_mb_init(&mailbox.mb, "mbt", &(mailbox.mb_pool[0]), sizeof(mailbox.mb_pool) / 4, RT_IPC_FLAG_FIFO);

    while (true) {
        if (rt_mb_recv(&mailbox.mb, (rt_uint32_t*)&psrt, RT_WAITING_FOREVER) == RT_EOK) {
            switch (psrt->sensor_type) {
                case 0x00:
                    do_processing_srt20x68(psrt->temp, psrt->dew);
                    break;
                case 0x01:
                    break;
            }
        }
    }
}

int dew_processing_init(void)
{
    rt_thread_init(&thread_dew_processing_handle, "sensor_task",
                   thread_entry_dew_processing, RT_NULL, thread_dew_processing_handle_stack,
                   sizeof(thread_dew_processing_handle_stack), 10,
                   5);
    rt_thread_startup(&thread_dew_processing_handle);
    return RT_EOK;
}

INIT_APP_EXPORT(dew_processing_init);

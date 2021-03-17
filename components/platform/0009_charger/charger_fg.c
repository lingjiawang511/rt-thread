#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "include/rtt_debug.h"

/* base on hw design: power/charger relate pins map:
 * PA8(pwr_on) PB1(charger_stat) PB0(CHARGER_CTL) PA9(CHARGER_DET_PIN) PD6(batt_ts) PA0(batt_voltage)  PA10(CHARGER_CLASIFY_PIN)
 * PIN67       PIN36             PIN35            PIN68                PIN87        PIN23              PIN69(need short to PA11(PIN70))
 */

typedef enum {
    fsm_rt_err     = -1,
    fsm_rt_cpl     = 0,
    fsm_rt_on_going = 1
} fsm_rt_t;

typedef enum {
    PWR_ON_PIN          = 0,
    CHARGER_STATUS_PIN  = 1,
    CHARGER_CTL_PIN     = 2,
    CHARGER_DET_PIN     = 3,
    BATTERY_TS_PIN      = 4,
    BATTERY_VOLTAGE_PIN = 5,
    CHARGER_CLASIFY_PIN = 6,
    PWR_CHARGER_PINS_MAX
} pin_enum;

typedef struct {
    uint32_t pin;
    uint32_t mode;
} pin_struct_t;

typedef enum {
    BQ24092_ILIMIT_100MA,
    BQ24092_ILIMIT_500MA,
    BQ24092_ILIMIT_RSET_UP_TO_1000MA
} bq_current_t;

typedef enum {
    CHARGER_IS_NONE,
    CHARGER_IS_USB,
    CHARGER_IS_DCP
} charget_type_t;

#define SHUTDOWN_RETRY_MAX  3
#define LOW_BATT_VALUE      2771 // (2.23/3.3*4095)
#define VBATT_HISTORY_MAX   5
#define USB_DM_PIN          70
#define RSA_DEBOUND_MAX     (5)
#undef  USE_POLLING_WORK

static pin_struct_t charger_pins[PWR_CHARGER_PINS_MAX] = {
    {67, PIN_MODE_OUTPUT},
    {36, PIN_MODE_INPUT},
    {35, PIN_MODE_OUTPUT},
    {68, PIN_MODE_INPUT_PULLDOWN},
    {87, PIN_MODE_INPUT},
    {23, PIN_MODE_INPUT},
    {69, PIN_MODE_OUTPUT_OD}
};

static const char * charge_type_msg[] = {
    "None",
    "USB",
    "DCP"
};

static  uint8_t  power_off_charge = 0x00;
static struct rt_event event = {0};

uint8_t get_power_off_charger(void)
{
    return power_off_charge ? 1 : 0;
}

void set_power_off_charger(uint8_t value)
{
    power_off_charge = value ? 1 : 0;
}

bq_current_t get_charger_current_limit(void)
{
    /* pin floatting means 100mA but we didn't use this mode */
    return rt_pin_read(charger_pins[CHARGER_CTL_PIN].pin) ? BQ24092_ILIMIT_500MA : BQ24092_ILIMIT_RSET_UP_TO_1000MA;
}

void set_charger_current_limit(bq_current_t s)
{
    switch (s) {
        case BQ24092_ILIMIT_100MA:
            /* set gpio to float input mode */
            rt_pin_mode(charger_pins[CHARGER_CTL_PIN].pin, PIN_MODE_INPUT);
            break;
        case BQ24092_ILIMIT_500MA:
            rt_pin_write(charger_pins[CHARGER_CTL_PIN].pin, 1);
            break;
        case BQ24092_ILIMIT_RSET_UP_TO_1000MA:
            rt_pin_write(charger_pins[CHARGER_CTL_PIN].pin, 0);
            break;
        default:
            rt_kprintf("[bq24092]invalid value:%d, fallback to default 500mA\n", s);
            rt_pin_write(charger_pins[CHARGER_CTL_PIN].pin, 1);
            break;
    }
}

uint8_t charger_is_connect(void)
{
    return rt_pin_read(charger_pins[CHARGER_DET_PIN].pin) ? 1 : 0;
}

uint8_t long_press_power_key(void)
{
    return 1;
}

uint8_t shutdown_device(const char *msg)
{
    rt_kprintf("%s", msg);
    rt_pin_write(charger_pins[PWR_ON_PIN].pin, 0);
    while (1) {
#if defined(__GNUC__)
        ;
#else
        __wfi();
#endif
    }
}

static rt_device_t adc0 = NULL;

uint32_t rt_adc_read(void)
{
    rt_uint32_t val;
    if (adc0) {
        rt_device_read(adc0, 0, &val, sizeof(val));
        return val;
    }
    return -1;
}

uint32_t fg_adc_device_find(const char *name)
{
    adc0 = (rt_device_t)rt_device_find(name);
    return rt_adc_read();
}

static rt_uint8_t thread_charger_handle_stack[1024];
static struct rt_thread thread_charger_handle;

static uint32_t adc_simple_filter(uint32_t *val, int n)
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

fsm_rt_t check_vbatt_low(uint32_t sample_time)
{
    static uint32_t vbatt_history[VBATT_HISTORY_MAX] = {0};
    static uint8_t history_index = 0;
    static uint8_t low_batt_count = 0;

    /*
     * If 100ms once on this function, check time is 10s in normal(after filter).
     * Assume low battery votage is 3.35v and we got 2.23v frm div resister.
     */

    /* sample voltage */
    vbatt_history[history_index] = rt_adc_read();
    if (++history_index < VBATT_HISTORY_MAX) {
        return fsm_rt_on_going;
    }
    history_index = 0x00;

    uint32_t val =  adc_simple_filter(vbatt_history, VBATT_HISTORY_MAX);

    if (val < LOW_BATT_VALUE) {
        if (++low_batt_count > SHUTDOWN_RETRY_MAX) {
            low_batt_count = 0x00;
            return fsm_rt_cpl;
        }
    } else {
        low_batt_count = 0x00;
    }
    return fsm_rt_on_going;
}

fsm_rt_t usb_enum_success(void)
{
    return fsm_rt_cpl;
}

void configure_gpio(uint32_t val)
{
    rt_pin_write(charger_pins[CHARGER_CLASIFY_PIN].pin, val);
}

fsm_rt_t dcp_enum_success(void)
{
    configure_gpio(0);
    if (0 == rt_pin_read(USB_DM_PIN)) {
        configure_gpio(1);
        if (1 == rt_pin_read(USB_DM_PIN)) {
            return fsm_rt_cpl;
        }
    }
    configure_gpio(1);
    return fsm_rt_err;
}

fsm_rt_t charger_work(charget_type_t *type, uint32_t context)
{
    enum {
        CHARGER_OFFLINE,
        CHARGER_ONLINE
    };
    static enum {
        STATE_CHECK_ONLINE,
        STATE_CHECK_DCP,
        STATE_CHECK_USB,
        STATE_CPL
    } s_state = STATE_CHECK_ONLINE;

    static uint8_t s_online =  0;
    uint8_t now_online =  0;

    switch (s_state) {
        case STATE_CHECK_ONLINE:
            now_online = charger_is_connect();
            if (now_online != s_online) {
                printk(0, "[charger] status change\n");
                s_online = now_online;
                if (!now_online) {
                    type[0] = type[1];
                    type[1] = CHARGER_IS_NONE;
                    s_state = STATE_CHECK_ONLINE;
                    return fsm_rt_cpl;
                }
                s_state = STATE_CHECK_DCP;
            }
            if (context) {
                return fsm_rt_err;
            }
            break;
        case STATE_CHECK_USB:
            if (fsm_rt_cpl == usb_enum_success()) {
                type[0] = type[1];
                type[1] = CHARGER_IS_USB;
                s_state = STATE_CHECK_ONLINE;
                return fsm_rt_cpl;
            }
            s_state = STATE_CHECK_ONLINE;
            break;
        case STATE_CHECK_DCP:
            if (fsm_rt_cpl == dcp_enum_success()) {
                type[0] = type[1];
                type[1] = CHARGER_IS_DCP;
                s_state = STATE_CHECK_ONLINE;
                return fsm_rt_cpl;
            }
            s_state = STATE_CHECK_USB;
            break;
        default:
            s_state = STATE_CHECK_ONLINE;
            break;
    }
    return fsm_rt_on_going;
}

static void usb_vbus_raising_falling_irq(void *p)
{
    rt_event_send(&event, 0x01);
}

static void thread_entry_charger_fg (void* parameter)
{
    /* charget_type[0] is history type, charget_type[1] is current type*/
    static charget_type_t charget_type[2] = {CHARGER_IS_NONE};

    for (int i = 0; i < PWR_CHARGER_PINS_MAX; i++) {
        rt_pin_mode(charger_pins[i].pin, charger_pins[i].mode);
    }

    rt_event_init(&event, "charger_event", RT_IPC_FLAG_FIFO);

    /* output 1 since this pin is OD */
    rt_pin_write(charger_pins[CHARGER_CLASIFY_PIN].pin, 1);
    rt_pin_attach_irq(charger_pins[CHARGER_DET_PIN].pin, PIN_IRQ_MODE_RISING_FALLING, usb_vbus_raising_falling_irq, charge_type_msg);
    rt_pin_irq_enable(charger_pins[CHARGER_DET_PIN].pin, PIN_IRQ_ENABLE);

    fg_adc_device_find("adc0");

    if (long_press_power_key()) {
        /* normal power on */
        rt_kprintf("[charger]normal power up\n");
        set_power_off_charger(0);
    } else {
        if (charger_is_connect()) {
            /* power off charger */
            rt_kprintf("[pwr]short press power key and charger online, powerof charge mode\n");
            set_power_off_charger(1);
        } else {
            //really shutdown meter
            shutdown_device("[pwr]short press power key and charger offline, shutdown...\n");
        }
    }
    /* power on the regulator */
    rt_pin_write(charger_pins[PWR_ON_PIN].pin, 1);
    /* set default charge current to 500mA */
    set_charger_current_limit(BQ24092_ILIMIT_500MA);

    while (1) {
#if defined(USE_POLLING_WORK)
        if (fsm_rt_cpl == check_vbatt_low(5)) {
            if (!charger_is_connect()) {
                set_charger_current_limit(BQ24092_ILIMIT_500MA);
                shutdown_device("[pwr]short press power key and charger offline, shutdown...\n");
            }
        }
        fsm_rt_t ret = charger_work(charget_type, 0);
        if (fsm_rt_cpl == ret) {
            printk(0, "[charger]mode change from %s to %s\n", charge_type_msg[charget_type[0]], charge_type_msg[charget_type[1]]);
            if (charget_type[1] == CHARGER_IS_DCP) {
                set_charger_current_limit(BQ24092_ILIMIT_RSET_UP_TO_1000MA);
            } else {
                set_charger_current_limit(BQ24092_ILIMIT_500MA);
            }
        }
        rt_thread_sleep(rt_tick_from_millisecond(100));
#else
        rt_uint32_t online = 0;
        rt_uint32_t count  = 0;
        printk(0, "[charger]wait event\n");
        rt_event_recv(&event, 0x01, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &online);
        count = 0;
        for (int i = 0; i < RSA_DEBOUND_MAX; i++) {
            if (charger_is_connect()) {
                count++;
            }
            rt_thread_sleep(rt_tick_from_millisecond(10));
        }

        if (count >= (RSA_DEBOUND_MAX - 2)) {
            online = 0x01;
        } else {
            online = 0x00;
        }

        printk(0, "[charger]got event, online=%d\n", online);

        if (0 == online) {
            charget_type[1] = CHARGER_IS_NONE;
            set_charger_current_limit(BQ24092_ILIMIT_500MA);
        } else if (fsm_rt_cpl == dcp_enum_success()) {
            charget_type[1] = CHARGER_IS_DCP;
            set_charger_current_limit(BQ24092_ILIMIT_RSET_UP_TO_1000MA);
        } else {
            charget_type[1] = CHARGER_IS_USB;
            set_charger_current_limit(BQ24092_ILIMIT_500MA);
        }

        if (charget_type[0] !=  charget_type[1]) {
            printk(0, "[charger]mode change from %s to %s\n", charge_type_msg[charget_type[0]], charge_type_msg[charget_type[1]]);
        }
        /* set last online value */
        charget_type[0] = charget_type[1];
#endif
    }
}

static int charger_init(void)
{
    rt_thread_init(&thread_charger_handle, "charger_fg_task",
                   thread_entry_charger_fg, RT_NULL, thread_charger_handle_stack,
                   sizeof(thread_charger_handle_stack), 10,
                   5);
    rt_thread_startup(&thread_charger_handle);
}

INIT_COMPONENT_EXPORT(charger_init);

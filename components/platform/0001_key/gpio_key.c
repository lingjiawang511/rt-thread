#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "multi_button.h"

#define RSA_MAX_BUTTON 7

/* base on hw design: key pins map:
 * PE0 PE1 PE2 PE3 PE4 PE5 PE6
 * 97  98  1   2   3   4   5
 */
static struct button btn[RSA_MAX_BUTTON] = {0};
static uint32_t key_pins[RSA_MAX_BUTTON] = {26, 27, 1, 2, 3, 34, 35};

static void button_callback_key1(void *btn);
static void button_callback_key2(void *btn);
static void button_callback_key3(void *btn);
static void button_callback_key4(void *btn);
static void button_callback_key5(void *btn);
static void button_callback_key6(void *btn);
static void button_callback_key7(void *btn);

static uint8_t button_read_pin_key1(void);
static uint8_t button_read_pin_key2(void);
static uint8_t button_read_pin_key3(void);
static uint8_t button_read_pin_key4(void);
static uint8_t button_read_pin_key5(void);
static uint8_t button_read_pin_key6(void);
static uint8_t button_read_pin_key7(void);

typedef void button_callback(void *btn);
typedef uint8_t button_read_pin(void);

button_callback  * callback[RSA_MAX_BUTTON] = {
    button_callback_key1,
    button_callback_key2,
    button_callback_key3,
    button_callback_key4,
    button_callback_key5,
    button_callback_key6,
    button_callback_key7
};

button_read_pin * read_pin[RSA_MAX_BUTTON] = {
    button_read_pin_key1,
    button_read_pin_key2,
    button_read_pin_key3,
    button_read_pin_key4,
    button_read_pin_key5,
    button_read_pin_key6,
    button_read_pin_key7
};

static uint8_t button_read_pin_key1(void)
{
    return rt_pin_read(key_pins[0]);
}
static uint8_t button_read_pin_key2(void)
{
    return rt_pin_read(key_pins[1]);
}
static uint8_t button_read_pin_key3(void)
{
    return rt_pin_read(key_pins[2]);
}
static uint8_t button_read_pin_key4(void)
{
    return rt_pin_read(key_pins[3]);
}
static uint8_t button_read_pin_key5(void)
{
    return rt_pin_read(key_pins[4]);
}
static uint8_t button_read_pin_key6(void)
{
    return rt_pin_read(key_pins[5]);
}
static uint8_t button_read_pin_key7(void)
{
    return rt_pin_read(key_pins[6]);
}

static void button_callback_key1(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch (btn_event_val) {
        case SINGLE_CLICK:
            rt_kprintf("button1 single click\n");
            break;
        case DOUBLE_CLICK:
            rt_kprintf("button1 double click\n");
            break;
        case LONG_PRESS_HOLD:
            rt_kprintf("button1 long press hold\n");
            break;
    }
}

static void button_callback_key2(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch (btn_event_val) {
        case SINGLE_CLICK:
            rt_kprintf("button2 single click\n");
            break;
        case DOUBLE_CLICK:
            rt_kprintf("button2 double click\n");
            break;
        case LONG_PRESS_HOLD:
            rt_kprintf("button2 long press hold\n");
            break;
    }
}

static void button_callback_key3(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch (btn_event_val) {
        case SINGLE_CLICK:
            rt_kprintf("button3 single click\n");
            break;
        case DOUBLE_CLICK:
            rt_kprintf("button3 double click\n");
            break;
        case LONG_PRESS_HOLD:
            rt_kprintf("button3 long press hold\n");
            break;
    }
}

static void button_callback_key4(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch (btn_event_val) {
        case SINGLE_CLICK:
            rt_kprintf("button4 single click\n");
            break;
        case DOUBLE_CLICK:
            rt_kprintf("button4 double click\n");
            break;
        case LONG_PRESS_HOLD:
            rt_kprintf("button4 long press hold\n");
            break;
    }
}

static void button_callback_key5(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch (btn_event_val) {
        case SINGLE_CLICK:
            rt_kprintf("button5 single click\n");
            break;
        case DOUBLE_CLICK:
            rt_kprintf("button5 double click\n");
            break;
        case LONG_PRESS_HOLD:
            rt_kprintf("button5 long press hold\n");
            break;
    }
}

static void button_callback_key6(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch (btn_event_val) {
        case SINGLE_CLICK:
            rt_kprintf("button6 single click\n");
            break;
        case DOUBLE_CLICK:
            rt_kprintf("button6 double click\n");
            break;
        case LONG_PRESS_HOLD:
            rt_kprintf("button6 long press hold\n");
            break;
    }
}

static void button_callback_key7(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch (btn_event_val) {
        case SINGLE_CLICK:
            rt_kprintf("button7 single click\n");
            break;
        case DOUBLE_CLICK:
            rt_kprintf("button7 double click\n");
            break;
        case LONG_PRESS_HOLD:
            rt_kprintf("button7 long press hold\n");
            break;
    }
}

/* use hardware timer if performance is not fit */
void btn_thread_entry(void* p)
{
    while (1) {
        /* 10ms */
        rt_thread_delay(RT_TICK_PER_SECOND / 100);
        button_ticks();
    }
}

int multi_button_init(void)
{
    rt_thread_t thread = RT_NULL;

    /* Create background ticks thread */
    thread = rt_thread_create("emwin", btn_thread_entry, RT_NULL, 1024, 12, 10);
    if (thread == RT_NULL) {
        return RT_ERROR;
    }
    rt_thread_startup(thread);

    /* low level drive */
    for (int i = 0; i < RSA_MAX_BUTTON; i++) {
        if (0 == key_pins[i]) {
            continue;
        }
        rt_pin_mode  (key_pins[i], PIN_MODE_INPUT_PULLUP);
        button_init  (&btn[i], read_pin[i], PIN_LOW);
        /*
         * button_attach(&btn[i], PRESS_DOWN,       button_callback);
         * button_attach(&btn[i], PRESS_UP,         button_callback);
         * button_attach(&btn[i], PRESS_REPEAT,     button_callback);
         */
        button_attach(&btn[i], SINGLE_CLICK,     callback[i]);
        button_attach(&btn[i], DOUBLE_CLICK,     callback[i]);
        /*
         *button_attach(&btn[i], LONG_RRESS_START, callback[i]);
         *button_attach(&btn[i], LONG_PRESS_HOLD,   callback[i]);
         */
        button_start (&btn[i]);
    }

    return RT_EOK;
}

INIT_APP_EXPORT(multi_button_init);

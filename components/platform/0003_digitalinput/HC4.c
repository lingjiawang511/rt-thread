#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"

typedef enum {
    fsm_rt_err     = -1,
    fsm_rt_cpl     = 0,
    fsm_rt_on_going = 1
} fsm_rt_t;

#define UART_RX_EVENT  (1<<0)

static struct rt_event event;
static rt_device_t uart_device = RT_NULL;

static rt_uint8_t thread_hc4_handle_stack[1024];
struct rt_thread thread_hc4_handle;

static rt_err_t uart_intput(rt_device_t dev, rt_size_t size)
{
    RT_ASSERT(dev != RT_NULL);

    /* release semaphore to let finsh thread rx data */
    rt_event_send(&event, UART_RX_EVENT);

    return RT_EOK;
}

static struct serial_configure hc4_ro_config = {
    BAUD_RATE_19200,   /* 9600 bits/s */
    DATA_BITS_8,      /* 8 databits */
    STOP_BITS_1,      /* 1 stopbit */
    PARITY_NONE,      /* No parity  */
    BIT_ORDER_LSB,    /* LSB first sent */
    NRZ_NORMAL,       /* Normal mode */
    512,             /* Buffer size */
    0
};

static rt_err_t uart_open(const char *name)
{
    rt_err_t res;
    /* 查找系统中的串口设备 */
    uart_device = rt_device_find(name);
    /* 查找到设备后将其打开 */
    if (uart_device != RT_NULL) {
        if (RT_EOK != rt_device_control(uart_device, RT_DEVICE_CTRL_CONFIG, (void *)&hc4_ro_config)) {
            rt_kprintf("uart config baud rate failed.\n");
        }
        res = rt_device_set_rx_indicate(uart_device, uart_intput);
        /* 检查返回值 */
        if (res != RT_EOK) {
            rt_kprintf("set %s rx indicate error.%d\n", name, res);
            return -RT_ERROR;
        }
        /* 打开设备，以可读写、中断方式 */
        res = rt_device_open(uart_device, RT_DEVICE_OFLAG_RDWR |
                             RT_DEVICE_FLAG_INT_RX );
        /* 检查返回值 */
        if (res != RT_EOK) {
            rt_kprintf("open %s device error.%d\n", name, res);
            return -RT_ERROR;
        }
    } else {
        rt_kprintf("can't find %s device.\n", name);
        return -RT_ERROR;
    }
    /* 初始化事件对象 */
    rt_event_init(&event, "event", RT_IPC_FLAG_FIFO);
    return RT_EOK;
}

static void uart_putchar(const rt_uint8_t c)
{
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;
    do {
        len = rt_device_write(uart_device, 0, &c, 1);
        timeout++;
    } while (len != 1 && timeout < 500);
}

static rt_uint8_t uart_getchar(void)
{
    rt_uint32_t e;
    rt_uint8_t ch;
    /* 读取 1 字节数据 */
    while (rt_device_read(uart_device, 0, &ch, 1) != 1) {
        /* 接收事件 */
        rt_event_recv(&event, UART_RX_EVENT, RT_EVENT_FLAG_AND |
                      RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &e);
    }
    return ch;
}

struct sensor_struct {
    int32_t sensor_type;
    int32_t dew;
    int32_t temp;
};

#define reset_hc4_ro_fsm() do{\
negative_flag = 0x00;\
var_srt.dew   = 0x00;\
var_srt.temp  = 0x00;\
s_state = RECV_NUM_T;\
}while(0)

static fsm_rt_t fsm_hc4_ro_recv(struct sensor_struct *p)
{

    static uint8_t negative_flag = 0;
    static struct sensor_struct var_srt = {0};

    static enum {
        RECV_CHAR_T,
        RECV_NUM_T,
        RECV_CHAR_V,
        RECV_NUM_V,
        RECV_CHAR_D,
        RECV_NUM_D,
    } s_state = RECV_CHAR_T;


    uint8_t ch = uart_getchar();

    switch (s_state) {
        case RECV_CHAR_T:
            if ('t' == ch) {
                s_state = RECV_NUM_T;
                negative_flag = 0;
            }
            break;
        case  RECV_NUM_T:
            if ('v' == ch) {
                s_state = RECV_NUM_V;
                break;
            }
            break;
        case  RECV_NUM_V:
            if ('d' == ch) {
                s_state = RECV_NUM_D;
                break;
            }
            if ('-' == ch) {
                negative_flag |= 1;
                break;
            }
            if ((ch > '9') || (ch < '0')) {
                reset_hc4_ro_fsm();
                break;
            }
            var_srt.temp *= 10;
            var_srt.temp += ch - '0';
            break;
        case  RECV_NUM_D:
            if ('t' == ch) {
                s_state = RECV_NUM_T;
                p->temp =  (0x01 & negative_flag) ? -var_srt.temp : var_srt.temp;
                p->temp *= 10;
                p->dew  =  (0x02 & negative_flag) ? -var_srt.dew : var_srt.dew;
                negative_flag = 0x00;
                var_srt.dew   = 0x00;
                var_srt.temp  = 0x00;
                return fsm_rt_cpl;
            }
            if ('-' == ch) {
                negative_flag |= 0x02;
                break;
            }
            if ((ch > '9') || (ch < '0')) {
                reset_hc4_ro_fsm();
                break;
            }
            var_srt.dew *= 10;
            var_srt.dew += ch - '0';
            break;
        default:
            reset_hc4_ro_fsm();
            break;
    }
    return fsm_rt_on_going;
}

static void thread_entry_hc4_ro (void* parameter)
{
    struct sensor_struct srt;
    if (RT_EOK != uart_open("uart1"))    {
        rt_kprintf("[%s] no uart device found, therad exit...\n", __FUNCTION__);
        return ;
    }
    /* 0x00 for hc4
     * 0x01 for hc4 sensor
     */
    srt.sensor_type = 0x01;
    while (1) {
        if (fsm_rt_cpl == fsm_hc4_ro_recv(&srt)) {
            rt_mb_send(get_dew_process_mailbox(), (rt_uint32_t)(&srt));
            //rt_kprintf("dew=%d temp=%d ", srt.dew, srt.temp);
        }
    }
}

static  int hc4_init(void)
{
    rt_thread_init(&thread_hc4_handle, "sensor_task",
                   thread_entry_hc4_ro, RT_NULL, thread_hc4_handle_stack,
                   sizeof(thread_hc4_handle_stack), 10,
                   5);
    rt_thread_startup(&thread_hc4_handle);
    return RT_EOK;
}

INIT_APP_EXPORT(hc4_init);

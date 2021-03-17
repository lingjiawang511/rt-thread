#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#if MB_SLAVE_RTU_ENABLED == 1
#include "mbrtu.h"
#endif
#if MB_SLAVE_ASCII_ENABLED == 1
#include "mbascii.h"
#endif
#if MB_SLAVE_TCP_ENABLED == 1
#include "mbtcp.h"
#endif

typedef enum {
    MB_PAR_NONE,                /*!< No parity. */
    MB_PAR_ODD,                 /*!< Odd parity. */
    MB_PAR_EVEN                 /*!< Even parity. */
} eMBParity;

typedef enum {
    MB_RTU,                     /*!< RTU transmission mode. */
    MB_ASCII,                   /*!< ASCII transmission mode. */
    MB_TCP                      /*!< TCP mode. */
} eMBMode;

#define thread_ModbusSlavePoll_Prio         10
static rt_uint8_t thread_ModbusSlavePoll_stack[512];
struct rt_thread thread_ModbusSlavePoll;

/**
* \brief Modbus??????
*/
void thread_entry_ModbusSlavePoll (void* parameter)
{
    // ????Port1 ?Uart1
    eMBInit(MB_RTU, 0x01, 0x02, 115200,  MB_PAR_NONE);
    eMBEnable();
    while (1) {
        eMBPoll();
    }
}

int test_modbus (void)
{
    rt_thread_init(&thread_ModbusSlavePoll, "MBSlavePoll",
                   thread_entry_ModbusSlavePoll, RT_NULL, thread_ModbusSlavePoll_stack,
                   sizeof(thread_ModbusSlavePoll_stack), thread_ModbusSlavePoll_Prio,
                   5);
    rt_thread_startup(&thread_ModbusSlavePoll);
    return RT_EOK;
}

INIT_APP_EXPORT(test_modbus);


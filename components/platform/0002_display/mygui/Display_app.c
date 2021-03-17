#include <rthw.h>
#include <rtthread.h>
#include "include/HeadType.h"
#include "stdlib.h"
#include "errno.h"
#include "stdlib.h"
#include "include/gui.h"

TagParam    param;
ModifyDataType ModifyMenuData;
unsigned long TimeInterval;
unsigned char dispaly_pwr_state;
TagWorkState     WorkState;
unsigned char dewoutofrangeFlag;
TagLang lang = LANGUAGE_ENGLISH;
TagRuntime* pruntime;
unsigned char SensorOnlineFlag;

static rt_uint8_t thread_display_handle_stack[512*4];
struct rt_thread thread_display_handle;
#define RT_DISPLAY_THREAD_PRIORITY				8

static TagRuntime runtime;
static TagParam param;

TagParam *rt_get_param_addr(void)
{
    return &param;
}

void set_runtime_param_len(int len)
{
    runtime.lenv_value = len;
}
void set_runtime_display_index(int index)
{
    runtime.display_index = index;
}
int get_runtime_param_len(void)
{
    return runtime.lenv_value;
}
int get_runtime_display_index(void)
{
    return runtime.display_index;
}
TagRuntime* get_runtime_value(void)
{
    return &runtime;
}

void thread_entry_display(void* parameter)
{

    pruntime = get_runtime_value();
    set_runtime_param_len(4);
    pruntime->temp_value = 100;
    pruntime->humi_value = 200;
    pruntime->drew_value = 1000;
    pruntime->ppmv_value = 1500;
    Gui_Lcd_Interface();
    while (1) {
        Display_Main_Param();
        rt_thread_mdelay(50);
    }
}

int display_init(void)
{
    rt_thread_init(&thread_display_handle, "display_task", thread_entry_display, RT_NULL, \
                   thread_display_handle_stack, sizeof(thread_display_handle_stack), \
										RT_DISPLAY_THREAD_PRIORITY, 5);
    rt_thread_startup(&thread_display_handle);
    return RT_EOK;
}

INIT_APP_EXPORT(display_init);


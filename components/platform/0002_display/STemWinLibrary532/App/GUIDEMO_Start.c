/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.32 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information

Licensor:                 SEGGER Software GmbH
Licensed to:              STMicroelectronics International NV
Licensed SEGGER software: emWin
License number:           GUI-00429
License model:            Buyout SRC [Buyout Source Code License, signed November 29th 2012]
Licensed product:         -
Licensed platform:        STMs ARM Cortex-M based 32 BIT CPUs
Licensed number of seats: -
----------------------------------------------------------------------
File        : GUIDEMO_Start.c
Purpose     : GUIDEMO initialization
----------------------------------------------------------------------
*/
#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "GUIDEMO.h"
#include "font_export.h"
/*********************************************************************
*
*       MainTask
*/
extern WM_HWIN CreateMainForm(void);
void MainTask(void)
{
#if GUI_WINSUPPORT
    WM_SetCreateFlags(WM_CF_MEMDEV);
#endif
    GUI_Init();
#if GUI_WINSUPPORT
    WM_MULTIBUF_Enable(1);
#endif
//    GUI_DispStringAt("hello world", 100, 100);
//    GUI_SetFont(&GUI_FontHZ12);
//    GUI_DispStringAt("�¶� world", 0, 0);
//    GUI_SetFont(&GUI_FontHZ16);
//    GUI_DispStringAt("�¶� world", 100, 150);
//    GUI_SetFont(&GUI_FontHZ24);
//    GUI_DispStringAt("�¶� world", 100, 200);
//    GUI_SetFont(&GUI_FontHZ32);
//    GUI_DispStringAt("�¶� worldabcdefrg", 100, 250);
//    CreateMainForm();//GUIDEMO_Main();
	 CreateFramewin(WM_HBKWIN);
	  while(1){
			GUI_Delay(20);
		}
}
/* use hardware timer if performance is not fit */
void emwin_thread_entry(void* p)
{
    MainTask();
}

int emwin_init(void)
{
    rt_thread_t thread = RT_NULL;

    /* Create background ticks thread */
    thread = rt_thread_create("emwin", emwin_thread_entry, RT_NULL, 1024 * 10, 12, 10);
    if (thread == RT_NULL) {
        return RT_ERROR;
    }
    rt_thread_startup(thread);

    return RT_EOK;
}

INIT_APP_EXPORT(emwin_init);
/*************************** End of file ****************************/


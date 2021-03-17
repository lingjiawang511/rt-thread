/*********************************************************************
*          Portions COPYRIGHT 2016 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
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
File        : GUIConf.c
Purpose     : Display controller initialization
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "GUI.h"
#include "rtthread.h"
#include "drivers/rt_drv_pwm.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// Define the available number of bytes available for the GUI
// use ocm as gui engine buffer, don't enable IRAM2 on keil's settings
#define GUI_NUMBYTES  (1024)*64       //modify by xd 0x10000000
#define GUI_BLOCKSIZE 0x80         // modify  by xd
void rt_device_probe_lcd(void);
void set_driver_template_lcdpdev(void *p);
void set_flexcolor_template_lcdpdev(void *p);
void GUI_X_SetBacklight(int val);
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void)
{
    //
    // 32 bit aligned memory area
    //
    // fix by wufeng. static U32 aMemory[GUI_NUMBYTES / 4];
    //
    // Assign memory to emWin
    //
    static U32 aMemory[GUI_NUMBYTES / 4] __attribute__((at(0x10000000)));
    GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
//    GUI_ALLOC_AssignMemory((U32 *)0x10000000, GUI_NUMBYTES);
    GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);   //modify  by xd
    //
    // Set default font
    //
    GUI_SetDefaultFont(GUI_FONT_6X8);
    rt_device_probe_lcd();
}

static struct rt_device * pBacklight = RT_NULL;
static const struct rt_pwm_configuration pwm_config = {
    .channel = 1,
    .period  = 1000000,
    .pulse   = 1000000 / 2
};

void rt_device_probe_lcd(void)
{
    struct rt_device * p = (struct rt_device *)rt_device_find("lcd");
    if (p) {
        set_driver_template_lcdpdev(p->user_data);
        set_flexcolor_template_lcdpdev(p->user_data);
    }
//    p = (struct rt_device *)rt_device_find("pwm3");
//    if (p) {
//        pBacklight = p;
//        rt_device_open(pBacklight, RT_DEVICE_FLAG_RDWR);
//        rt_device_control(pBacklight, PWM_CMD_SET, &pwm_config);
//        rt_pwm_enable((struct rt_device_pwm *)pBacklight, pwm_config.channel);
//        GUI_X_SetBacklight(50);
//    }
}

void GUI_X_SetBacklight(int val)
{
    rt_uint32_t pulse = val * pwm_config.period / 100;

    if (pBacklight) {
        rt_device_write(pBacklight, pwm_config.channel, &pulse, sizeof(pulse));
    }
}
/*************************** End of file ****************************/

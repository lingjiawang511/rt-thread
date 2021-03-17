//#include "DIALOG.h"
//#include "font_export.h"
//#define ID_FRAMEWIN_0  (GUI_ID_USER + 0x00)
//#define ID_CHECKBOX_0  (GUI_ID_USER + 0x01)
//#define ID_GRAPH_0  (GUI_ID_USER + 0x02)
//#define ID_BUTTON_0  (GUI_ID_USER + 0x03)
//#define ID_BUTTON_1  (GUI_ID_USER + 0x04)
//#define ID_BUTTON_2  (GUI_ID_USER + 0x05)
//#define ID_BUTTON_3  (GUI_ID_USER + 0x06)


//static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
//	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 480, 800, 0, 0x64, 0 },
//	{ GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 15, 25, 440, 300, 0, 0x0, 0 },
//	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 15, 330, 90, 30, 0, 0x0, 0 },
//	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 130, 330, 90, 30, 0, 0x0, 0 },
//	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_2, 250, 330, 90, 30, 0, 0x0, 0 },
//	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_3, 365, 330, 90, 30, 0, 0x0, 0 },
//	{ CHECKBOX_CreateIndirect, "Checkbox", ID_CHECKBOX_0, 195, 5, 100, 16, 0, 0x0, 0 },
//};

//#define MAX_VALUE	125
//static GRAPH_DATA_Handle  pdataGRP;

//static void _cbDialog(WM_MESSAGE * pMsg) {
//	WM_HWIN hItem;
//	int     NCode;
//	int     Id;

//	short Val;
//	GRAPH_SCALE_Handle hScaleV;
//	GRAPH_SCALE_Handle hScaleH;

//	switch (pMsg->MsgId) {
//	case WM_INIT_DIALOG://��ʼ����Ϣ,��������/�ؼ�ʱ��Ч,��������������һЩ�ؼ��ĳ�ʼ����
//		hItem = pMsg->hWin;
//		FRAMEWIN_SetText(hItem, "www.neqee.com");
//		FRAMEWIN_SetTextColor(hItem, (0x00FF0000));
//		FRAMEWIN_SetTitleHeight(hItem, 20);
//		FRAMEWIN_SetFont(hItem, &GUI_FontHZ16);

//		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
//		CHECKBOX_SetText(hItem, "TIMER OFF");
//		CHECKBOX_SetFont(hItem, &GUI_FontHZ12);
//		CHECKBOX_SetTextColor(hItem, (0x008000FF));

//		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
//		BUTTON_SetFont(hItem, &GUI_FontHZ16);
//		BUTTON_SetText(hItem, "Button0");

//		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
//		BUTTON_SetFont(hItem, &GUI_FontHZ16);
//		BUTTON_SetText(hItem, "Button1");

//		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
//		BUTTON_SetFont(hItem, &GUI_FontHZ16);
//		BUTTON_SetText(hItem, "Button2");

//		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
//		BUTTON_SetFont(hItem, &GUI_FontHZ16);
//		BUTTON_SetText(hItem, "Button3");

//		hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
//		GRAPH_SetBorder(hItem, 40, 5, 5, 30);
//		GRAPH_SetGridVis(hItem, 1);
//		GRAPH_SetGridFixedX(hItem, 1);
//		GRAPH_SetGridDistY(hItem, 25);
//		GRAPH_SetGridDistX(hItem, 50);
//		hScaleV = GRAPH_SCALE_Create(30, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 25);//���������Ӵ�ֱ��Χ�߶ȱ�ǩ
//		GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);									//���ñ�ǩ������ɫ
//		GRAPH_AttachScale(hItem, hScaleV);											//����ǩ��ӵ���ֱ����
//		hScaleH = GRAPH_SCALE_Create(280, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);	//����������ˮƽ��Χ�߶ȱ�ǩ
//		GRAPH_SCALE_SetTextColor(hScaleH, GUI_DARKGREEN);							//����������ɫ
//		GRAPH_AttachScale(hItem, hScaleH);											//��ӵ�ˮƽ����
//		pdataGRP = GRAPH_DATA_YT_Create(GUI_GREEN, 50/*������ݸ���*/, 0, 0);		//����һ����������,�ɴ����������
//		GRAPH_AttachData(hItem, pdataGRP);											//Ϊ��ͼ�ؼ�������ݶ���
//		break;
//	case WM_KEY://���̰�����Ϣ(����ʵ����̳����ܵ�����)
//		Val = (short)(((WM_KEY_INFO*)(pMsg->Data.p))->Key);	//��ȡ��ֵ
//		GRAPH_DATA_YT_AddValue(pdataGRP, (Val>MAX_VALUE) ? MAX_VALUE : Val);//��ֵ������
//		break;
//	case WM_TIMER://��ʱ����Ϣ(��ʱ��ʱ�����ܵ�����)
//		WM_RestartTimer(pMsg->Data.v, 300);
//		if(WM_IsCompletelyCovered(pMsg->hWin)) break;		//���л�������ҳ��ʲô������

//		//�����������ȡADCֵ
//		if(!CHECKBOX_IsChecked(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0)))
//			GRAPH_DATA_YT_AddValue(pdataGRP, (I16)50);		//��ֵ������
//		break;
//	case WM_NOTIFY_PARENT://����������Ϣ����(������Ļ������ܵ�����),����������������༭��(�κεĲ���)�ȵ�......
//		Id    = WM_GetId(pMsg->hWinSrc);
//		NCode = pMsg->Data.v;
//		switch(Id) {
//		case ID_CHECKBOX_0:
//			switch(NCode) {
//			case WM_NOTIFICATION_CLICKED:
//			case WM_NOTIFICATION_RELEASED:
//			case WM_NOTIFICATION_VALUE_CHANGED:
//				WM_SetFocus(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0));	//�۽���ID_BUTTON_0�ؼ�
//				break;
//			}
//			break;
//		case ID_BUTTON_0:
//			switch(NCode) {
//			case WM_NOTIFICATION_CLICKED: break;
//			case WM_NOTIFICATION_RELEASED:
//				GRAPH_DATA_YT_AddValue(pdataGRP, (I16)0);	//��ֵ������
//				break;
//			}
//			break;
//		case ID_BUTTON_1:
//			switch(NCode) {
//			case WM_NOTIFICATION_CLICKED: break;
//			case WM_NOTIFICATION_RELEASED:
//				GRAPH_DATA_YT_AddValue(pdataGRP, (I16)25);	//��ֵ������
//				break;
//			}
//			break;
//		case ID_BUTTON_2:
//			switch(NCode) {
//			case WM_NOTIFICATION_CLICKED: break;
//			case WM_NOTIFICATION_RELEASED:
//				GRAPH_DATA_YT_AddValue(pdataGRP, (I16)75);	//��ֵ������
//				break;
//			}
//			break;
//		case ID_BUTTON_3:
//			switch(NCode) {
//			case WM_NOTIFICATION_CLICKED: break;
//			case WM_NOTIFICATION_RELEASED:
//				GRAPH_DATA_YT_AddValue(pdataGRP, (I16)100);	//��ֵ������
//				break;
//			}
//			break;
//		}
//		break;
//	default:
//		WM_DefaultProc(pMsg);
//		break;
//	}
//}

//WM_HWIN CreateFramewin(void);
//WM_HWIN CreateFramewin(void) {
//  WM_HWIN hWin;
//  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
//  WM_CreateTimer(WM_GetClientWindow(hWin), 1, 300, 0); //����һ�������ʱ��
//  return hWin;
//}
/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.30                          *
*        Compiled Jul  1 2015, 10:50:32                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
// USER END

#include "DIALOG.h"
#include "font_export.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_GRAPH_0 (GUI_ID_USER + 0x01)
//#define ID_BUTTON_0 (GUI_ID_USER + 0x02)


// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
	{ WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 480, 800, 0, 0x0, 0 },
	{ GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 370, 480, 430, 0, 0x0, 0 },
//	{ WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 370, 480, 430, 0, 0x0, 0 },
//  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 0, 0, 480, 430, 0, 0x0, 0 },
//  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 14, 330, 80, 35, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
#define MAX_VALUE	125
static GRAPH_DATA_Handle  pdataGRP;
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
	short Val;
	GRAPH_SCALE_Handle hScaleV;
	GRAPH_SCALE_Handle hScaleH;

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Graph'
    //
	  hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00FF4A4A));
//	  GUI_DispStringAt("hello world", 100, 100);
//    GUI_SetFont(&GUI_FontHZ12);
//    GUI_DispStringAt("�¶� world", 0, 0);
//    GUI_SetFont(&GUI_FontHZ16);
//    GUI_DispStringAt("�¶� world", 100, 150);
//    GUI_SetFont(&GUI_FontHZ24);
//    GUI_DispStringAt("�¶� world", 100, 200);
//    GUI_SetFont(&GUI_FontHZ32);
//    GUI_DispStringAt("�¶� worldabcdefrg", 100, 250);
	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
		GRAPH_SetBorder(hItem, 20, 5, 5, 20);
		GRAPH_SetGridVis(hItem, 1);
		GRAPH_SetGridFixedX(hItem, 1);
		GRAPH_SetGridDistY(hItem, 25);
		GRAPH_SetGridDistX(hItem, 50);
		hScaleV = GRAPH_SCALE_Create(19, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 25);//���������Ӵ�ֱ��Χ�߶ȱ�ǩ
		GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);									//���ñ�ǩ������ɫ
		GRAPH_AttachScale(hItem, hScaleV);											//����ǩ��ӵ���ֱ����
		hScaleH = GRAPH_SCALE_Create(415, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);	//����������ˮƽ��Χ�߶ȱ�ǩ
		GRAPH_SCALE_SetTextColor(hScaleH, GUI_DARKGREEN);							//����������ɫ
		GRAPH_AttachScale(hItem, hScaleH);//��ӵ�ˮƽ����
		
	  pdataGRP = GRAPH_DATA_YT_Create(GUI_RED, 10/*������ݸ���*/, 0, 0);		//����һ����������,�ɴ����������
		GRAPH_AttachData(hItem, pdataGRP);	
    break;
	case WM_TIMER://��ʱ����Ϣ(��ʱ��ʱ�����ܵ�����)
		static I16 testd = 5;
		WM_RestartTimer(pMsg->Data.v, 300);
//		if(WM_IsCompletelyCovered(pMsg->hWin)) break;		//���л�������ҳ��ʲô������

		//�����������ȡADCֵ
//		if(!CHECKBOX_IsChecked(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0)))
			GRAPH_DATA_YT_AddValue(pdataGRP, (I16)testd);		//��ֵ������
	    if(testd > 300){
				testd = testd -100;
			}else{
				testd = (testd+1)*3 - (testd-1)*2;
			}
	    if(testd > 450){
				testd = 10;
			}
		break;	
//  case WM_NOTIFY_PARENT:
//    Id    = WM_GetId(pMsg->hWinSrc);
//    NCode = pMsg->Data.v;
//    switch(Id) {
//    case ID_BUTTON_0: // Notifications sent by 'Button'
//      switch(NCode) {
//      case WM_NOTIFICATION_CLICKED:
//        // USER START (Optionally insert code for reacting on notification message)
//        // USER END
//        break;
//      case WM_NOTIFICATION_RELEASED:
//        // USER START (Optionally insert code for reacting on notification message)
//        // USER END
//        break;
//      // USER START (Optionally insert additional code for further notification handling)
//      // USER END
//      }
//      break;
//    // USER START (Optionally insert additional code for further Ids)
//    // USER END
//    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateWindow
*/
//WM_HWIN CreateWindow(void);
//WM_HWIN CreateWindow(void) {
WM_HWIN CreateFramewin(void);
WM_HWIN CreateFramewin(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  WM_CreateTimer(WM_GetClientWindow(hWin), 1, 300, 0); //����һ�������ʱ��
	return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/

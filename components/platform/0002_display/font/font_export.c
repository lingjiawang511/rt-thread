#include "rtthread.h"
//#include "include/init.h"
#include "font_export.h"
#include "EmWin_ASCII_Font.h"
#include "GUI_Private.h"
//#define RSA_ARRAY_MAX (1024*1014*8)
//#define QSPI_ROM_BASE_ADDR (0x90000000)

//USER_QSPI_BANK_ROM(unsigned char, gb2312_16x16[RSA_ARRAY_MAX]) = {0x00};

//unsigned long get_bank_checksum(void)
//{
//    unsigned long sum = 0;
//    for (int i = 0; i < RSA_ARRAY_MAX; i++) {
//        sum += gb2312_16x16[i];
//    }
//    return sum;
//}

//��ģ�����ݴ�����,������ģ������ֽ���
//������ģ���Ϊ1024
#define BYTES_PER_FONT  1024    //32*32=1024�������ʾ32*32�����ֿ�
U8 GUI_FontDataBuf[BYTES_PER_FONT];
#define FONT_GET_EXROM_DATA     0
void GUI_DrawFont_GetData (U8 *readdatabuf, U16P foncode, U8 size);

//��ȡ��ģ����
static void GUI_GetDataFromMemory(const GUI_FONT_PROP GUI_UNI_PTR *pProp, U16P c)
{
    unsigned char qh, ql;
    unsigned char i;
    unsigned long foffset;
    unsigned long t;
    unsigned char *mat;
    U16 size, csize; //�����С
    U16 BytesPerFont;
    GUI_FONT EMWINFONT;
    EMWINFONT = *GUI_pContext->pAFont;
    BytesPerFont = GUI_pContext->pAFont->YSize * pProp->paCharInfo->BytesPerLine; //ÿ����ģ�������ֽ���
    if (BytesPerFont > BYTES_PER_FONT) BytesPerFont = BYTES_PER_FONT;

    //�ж�����Ĵ�С
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_12)
    if (memcmp(&EMWINFONT, &GUI_FontHZ12, sizeof(GUI_FONT)) == 0) size = 12;    //12����
#endif
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_16)
    if (memcmp(&EMWINFONT, &GUI_FontHZ16, sizeof(GUI_FONT)) == 0) size = 16; //16����
#endif
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_24)
    if (memcmp(&EMWINFONT, &GUI_FontHZ24, sizeof(GUI_FONT)) == 0) size = 24; //24����
#endif
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_32)
    if (memcmp(&EMWINFONT, &GUI_FontHZ32, sizeof(GUI_FONT)) == 0) size = 32; //32����
#endif
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_48)
    if (memcmp(&EMWINFONT, &GUI_FontHZ48, sizeof(GUI_FONT)) == 0) size = 48; //48����
#endif
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_64)
    if (memcmp(&EMWINFONT, &GUI_FontHZ64, sizeof(GUI_FONT)) == 0) size = 64; //64����
#endif
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_80)
    if (memcmp(&EMWINFONT, &GUI_FontHZ80, sizeof(GUI_FONT)) == 0) size = 80; //80����
#endif
    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size); //�õ�����һ���ַ���Ӧ������ռ���ֽ���
    if (c < 0x80) { //Ӣ���ַ���ַƫ���㷨
        switch (size) {
            case 12:
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_12)
                for (t = 0; t < 12; t++) GUI_FontDataBuf[t] = emwin_asc2_1206[c - 0x20][t];
#endif
                break;
            case 16:
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_16)
                for (t = 0; t < 16; t++) GUI_FontDataBuf[t] = emwin_asc2_1608[c - 0x20][t];
#endif
                break;
            case 24:
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_24)
                for (t = 0; t < 48; t++) GUI_FontDataBuf[t] = emwin_asc2_2412[c - 0x20][t];
#endif
                break;
            case 32:
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_32)
                for (t = 0; t < 64; t++) GUI_FontDataBuf[t] = emwin_asc2_3216[c - 0x20][t];
#endif
                break;
            case 48: //�����ֿ�ʼ��Ŀǰֻ�����֣���Ҫ�������ַ���ʱ���ټ�������������
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_48)
                for (t = 0; t < 144; t++) GUI_FontDataBuf[t] = emwin_asc2_4824[c - 0x30][t];
#endif
                break;
            case 64: //�����ֿ�ʼ��Ŀǰֻ�����֣���Ҫ�������ַ���ʱ���ټ�������������
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_64)
                for (t = 0; t < 256; t++) GUI_FontDataBuf[t] = emwin_asc2_6432[c - 0x30][t];
#endif
                break;
            case 80: //�����ֿ�ʼ��Ŀǰֻ�����֣���Ҫ�������ַ���ʱ���ټ�������������
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_80)
                for (t = 0; t < 256; t++) GUI_FontDataBuf[t] = emwin_asc2_8040[c - 0x30][t];
#endif
                break;
        }
    } else {
#if FONT_GET_EXROM_DATA
        ql = c / 256;
        qh = c % 256;
        if (qh < 0x81 || ql < 0x40 || ql == 0xff || qh == 0xff) { //�ǳ��ú���
            for (i = 0; i < (size * 2); i++) *mat++ = 0x00; //�������
            return; //��������
        }
        if (ql < 0x7f)ql -= 0x40;
        else ql -= 0x41;
        qh -= 0x81;
        foffset = ((unsigned long)190 * qh + ql) * csize; //�õ��ֿ��е��ֽ�ƫ����
        switch (size) {
            case 12:
                W25QXX_Read(GUI_FontDataBuf, foffset + ftinfo.f12addr, csize);
                break;
            case 16:
                W25QXX_Read(GUI_FontDataBuf, foffset + ftinfo.f16addr, csize);
                break;
            case 24:
                W25QXX_Read(GUI_FontDataBuf, foffset + ftinfo.f24addr, csize);
                break;
        }
#else
        GUI_DrawFont_GetData(GUI_FontDataBuf, c, size);
#endif
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
*       GUIPROP_DispChar
*
* Purpose:
*   This is the routine that displays a character. It is used by all
*   other routines which display characters as a subroutine.
*/
void GUIPROP_X_DispChar(U16P c)
{
    int BytesPerLine;
    GUI_DRAWMODE DrawMode = GUI_pContext->TextMode;
    const GUI_FONT_PROP GUI_UNI_PTR *pProp = GUI_pContext->pAFont->p.pProp;
    //������λ�ֿ�������Ϣ
    for (; pProp; pProp = pProp->pNext) {
        if ((c >= pProp->First) && (c <= pProp->Last)) break;
    }
    if (pProp) {
        GUI_DRAWMODE OldDrawMode;
        const GUI_CHARINFO GUI_UNI_PTR * pCharInfo = pProp->paCharInfo;
        GUI_GetDataFromMemory(pProp, c);//ȡ����ģ����
        BytesPerLine = pCharInfo->BytesPerLine;
        OldDrawMode  = LCD_SetDrawMode(DrawMode);
        LCD_DrawBitmap(GUI_pContext->DispPosX, GUI_pContext->DispPosY,
                       pCharInfo->XSize, GUI_pContext->pAFont->YSize,
                       GUI_pContext->pAFont->XMag, GUI_pContext->pAFont->YMag,
                       1,     /* Bits per Pixel */
                       BytesPerLine,
                       &GUI_FontDataBuf[0],
                       &LCD_BKCOLORINDEX
                      );
        /* Fill empty pixel lines */
        if (GUI_pContext->pAFont->YDist > GUI_pContext->pAFont->YSize) {
            int YMag = GUI_pContext->pAFont->YMag;
            int YDist = GUI_pContext->pAFont->YDist * YMag;
            int YSize = GUI_pContext->pAFont->YSize * YMag;
            if (DrawMode != LCD_DRAWMODE_TRANS) {
                LCD_COLOR OldColor = GUI_GetColor();
                GUI_SetColor(GUI_GetBkColor());
                LCD_FillRect(GUI_pContext->DispPosX, GUI_pContext->DispPosY + YSize,
                             GUI_pContext->DispPosX + pCharInfo->XSize,
                             GUI_pContext->DispPosY + YDist);
                GUI_SetColor(OldColor);
            }
        }
        LCD_SetDrawMode(OldDrawMode); /* Restore draw mode */
        GUI_pContext->DispPosX += pCharInfo->XDist * GUI_pContext->pAFont->XMag;
    }
}

/*********************************************************************
*
*       GUIPROP_GetCharDistX
*/
int GUIPROP_X_GetCharDistX(U16P c)
{
    const GUI_FONT_PROP GUI_UNI_PTR * pProp = GUI_pContext->pAFont->p.pProp;
    for (; pProp; pProp = pProp->pNext) {
        if ((c >= pProp->First) && (c <= pProp->Last))break;
    }
    return (pProp) ? (pProp->paCharInfo)->XSize * GUI_pContext->pAFont->XMag : 0;
}

void GUI_DrawFont_GetData(U8 *readdatabuf, U16P foncode, U8 size)
{

    U16 i, csize, k;
    U16 HZnum;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size); //�õ�����һ���ַ���Ӧ������ռ���ֽ���

#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_12)
    if (size == 12) {
        HZnum = sizeof ( tfont12 ) / sizeof ( typFNT_GB12 );    //�Զ�ͳ�ƺ�����Ŀ
        for ( k = 0; k < HZnum; k++ ) {
            //        if ( ( tfont24[k].Index[0] == * ( foncode ) ) && ( tfont24[k].Index[1] == * ( foncode + 1 ) ) ) {
            if ( tfont12[k].Index[0] + tfont12[k].Index[1] * 256 ==  foncode ) {
                break;
            }
        }
        if (k == HZnum) { //û�ҵ����֣�����
            return;
        }
        for (i = 0; i < csize; i++) {
            readdatabuf[i] = tfont12[k].Msk[i];
        }
    }
#endif
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_16)
    if (size == 16) {
        HZnum = sizeof ( tfont16 ) / sizeof ( typFNT_GB16 );    //�Զ�ͳ�ƺ�����Ŀ
        for ( k = 0; k < HZnum; k++ ) {
            //        if ( ( tfont24[k].Index[0] == * ( foncode ) ) && ( tfont24[k].Index[1] == * ( foncode + 1 ) ) ) {
            if ( tfont16[k].Index[0] + tfont16[k].Index[1] * 256 ==  foncode ) {
                break;
            }
        }
        if (k == HZnum) { //û�ҵ����֣�����
            return;
        }
        for (i = 0; i < csize; i++) {
            readdatabuf[i] = tfont16[k].Msk[i];
        }
    }
#endif
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_24)
    if (size == 24) {
        HZnum = sizeof ( tfont24 ) / sizeof ( typFNT_GB24 );    //�Զ�ͳ�ƺ�����Ŀ
        for ( k = 0; k < HZnum; k++ ) {
            //        if ( ( tfont24[k].Index[0] == * ( foncode ) ) && ( tfont24[k].Index[1] == * ( foncode + 1 ) ) ) {
            if ( tfont24[k].Index[0] + tfont24[k].Index[1] * 256 ==  foncode ) {
                break;
            }
        }
        if (k == HZnum) { //û�ҵ����֣�����
            return;
        }
        for (i = 0; i < csize; i++) {
            readdatabuf[i] = tfont24[k].Msk[i];
        }
    }
#endif
#if defined(RT_USING_COMPONENTS_METER_DISPLAY_FONT_32)
    if (size == 32) {
        HZnum = sizeof ( tfont32 ) / sizeof ( typFNT_GB32 );    //�Զ�ͳ�ƺ�����Ŀ
        for ( k = 0; k < HZnum; k++ ) {
            if ( tfont32[k].Index[0] + tfont32[k].Index[1] * 256 ==  foncode ) {
                break;
            }
        }
        if (k == HZnum) { //û�ҵ����֣�����
            return;
        }
        for (i = 0; i < csize; i++) {
            readdatabuf[i] = tfont32[k].Msk[i];
        }
    }
#endif
}


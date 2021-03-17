#include <stdint.h>
#include <stdio.h>
#include "errno.h"
#if defined(__CC_ARM)
#elif defined(__GNUC__)
#include "sys/types.h"
#endif
#include "include/mystdio.h"
#include "include/mystring.h"
#include "include/HeadType.h"
#include "include/gui.h"

int32_t sched_get_current()
{
	return 100000;
}
extern u8 dispaly_pwr_state;
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE {
    int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    return ch;
}
#endif
int putchar1(int ch)  //Debug
{
//  GkUartWriteByte((char)ch);
//  if (ch == '\n')
//  GkUartWriteByte('\r');
    /*
    if (ch == '\n')
        uart_send_byte_lowlevel('\r');
    uart_send_byte_lowlevel(ch);*/
    return ch;
}

#if 0
//fixme: filter control word.
int getchar(void)
{
    int ch;

    ch = raw_getchar();
    putchar1(ch);

    return ch;
}

int raw_getchar(void)
{
//  return GkUartReadByte();
    return 0;//debug
}

int puts(const char * str)
{
    int ret;

    while (*str) {
        ret = putchar1(*str++);
        if (-1 == ret)
            return -1;
    }

    return (putchar1('\n'));
}


char *gets(char *s)
{
    char *str;

    str = s;

    while (1) {
        *str = raw_getchar();

        if (*str == '\r' || *str == '\n')
            break;

#ifndef GBIOS_VERSION
        if (*str == '\b' && str > s)
#else
        if (*str == (char)127)
#endif
        {
            if (str > s) {
                putchar1('\b');
                putchar1(' ');
                putchar1('\b');

                str--;
            }
        } else if (*str >= ' ' && *str < 127) {
            putchar1(*str++);
        }
    }

    *str = '\0';

    return s;
}
#endif


/**
    vsprintf() sign contain:
        '-':align left
        '+':if number is positive, output '+' before it, if number is negative, output '-' before it.
        ' ':(space)if number is positive, output ' ' before it,(but not output sign'+'),if it is negative, output sign before space.
        '#':if output oct, '0' before it,if output hex, '0x' before it.
        '0':fill '0' instead of ' ' for all type.if '-' occurs , ignore '0';
        '*':assign paramenter to align right
*/
#define LEFT        0x1
#define SIGN        0x2
#define SPACE       0x4
#define PLUS        0x8
#define ZERO        0x10
#define SIGNINT     0x20


//#define ISDIGIT(x) (x >= '0' && x <= '9')

static char *NumToAscii(char *pBuf, \
                        unsigned long nNum, \
                        int nWidth, \
                        int nBase, \
                        int nOutputStyle)
{
    char chTempArray[32], chFillChar, chSign = '\0';
    const char *pchDigit = "0123456789abcdef";
    int nCount = 0;
    int nPrefixNum = 0;


    chFillChar = (nOutputStyle & ZERO) ? '0' : ' ';//set ZERO attribute

    //set SIGN and SPACE
    if (nOutputStyle & SIGN && (10 == nBase)) {
        if (nNum & 0x80000000) {
            chSign = '-';
            nNum = -nNum;
        } else if (!(nOutputStyle & SIGNINT)) {
            chSign = '+';
        }
    } else if (!((nOutputStyle & SIGN) || (nNum & (0x80000000))) && (10 == nBase) && (nOutputStyle & SPACE))
        chSign = ' ';

    //if num == 0
    if (nNum == 0)
        chTempArray[nCount++] = '0';

    //convert number
    while (nNum != 0) {
        chTempArray[nCount++] = pchDigit[nNum % nBase];
        nNum = nNum / nBase;
    }

    //set PLUS
    if (nOutputStyle & PLUS) {
        if (nBase == 16)//handle hex
            nPrefixNum = 2;
        if (nBase == 8)//handle oct
            nPrefixNum = 1;
    }

    //EccGenerate number of chFillChar
    nWidth = nWidth - nPrefixNum - (chSign ? 1 : 0) - nCount;

    if (!(nOutputStyle & LEFT)) {
        while (nWidth-- > 0)
            *pBuf++ = chFillChar;
    }

    switch (nPrefixNum) {
        case 2:
            *pBuf++ = '0';
            *pBuf++ = 'x';
            break;
        case 1:
            *pBuf++ = '0';
            break;
        default :
            break;
    }

    if (chSign)
        *pBuf++ = chSign;

    while (nCount > 0)
        *pBuf++ = chTempArray[--nCount];

    while (nWidth-- > 0)
        *pBuf++ = chFillChar;

    return pBuf;
}


static int vsprintk(char *pchBuf, const char *pchFormat, int *pPara)
{
    int nOutputStyle, nWidth, nBase;
    const char *pchFmtRoll;
    char *pchStrTemp;
    int nStrLen;
    char chTempArray[32];
    int nCount = 0;
    char *pchBufTemp = pchBuf;

    while (*pchFormat) {
        if ('%' != *pchFormat) {
            *pchBufTemp++ = *pchFormat++;
            continue;
        }
        pchFmtRoll = pchFormat++;

        if ('%' == *pchFormat) { //handle e.g "%%"
            *pchBufTemp++ = *pchFormat++;
            continue;
        }
        pchFormat--;

        nOutputStyle = 0;
    get_sign :
        pchFormat++;//skip '%', handle next charactor, and record current position
        switch (*pchFormat) {
            case '-' :
                nOutputStyle |= LEFT;
                nOutputStyle &= (~ZERO);
                goto get_sign;
            case '+' :
                nOutputStyle |= SIGN;
                goto get_sign;
            case ' ' :
                nOutputStyle |= SPACE;
                goto get_sign;
            case '#' :
                nOutputStyle |= PLUS;
                goto get_sign;
            case '0' :
                if (!(nOutputStyle & LEFT))
                    nOutputStyle |= ZERO;
                goto get_sign;
        }

        nWidth = 0;

        while (ISDIGIT(*pchFormat)) {
            nWidth = nWidth * 10 + *pchFormat - '0';
            pchFormat++;
        }

        //handle '*',replace '*'
        if ('*' == *pchFormat) {
            pchFormat++;

            if (!ISDIGIT(*pchFormat)) {
                nOutputStyle &= ~LEFT;
                nWidth = *pPara++;
            } else {
                while (*pPara) {
                    chTempArray[nCount++] = *pPara % 10 + '0';
                    *pPara /= 10;
                }

                while (nCount)
                    (*pchBufTemp++) = chTempArray[--nCount];

                pPara++;
                continue;
            }
        }

        nBase = 10;

        //fixme:handle '%l(o,x,i,d)' and '%ll(o,x,i,d)',filter 'l' and 'll'
        if (('l' == *pchFormat) && ('l' == *++pchFormat))
            *pchFormat ++;

        switch (*pchFormat) {
            case 'u':
                nBase = 10;
                nOutputStyle &= ~(SIGN | PLUS);
                break;

            case 'd' :
            case 'i' :
                nBase = 10;
                if (!(nOutputStyle & SIGN))
                    nOutputStyle |= SIGNINT | SIGN;
//          nOutputStyle &= (~PLUS);
                break;
            case 'p' :
                nOutputStyle |= PLUS;
            case 'X' :
            case 'x' :
                nBase = 16;
//          nOutputStyle &= (~(SPACE | SIGN));
                break;
            case 'o' :
                nBase = 8;
//          nOutputStyle &= (~(SPACE | SIGN));
                break;
            case 's' :
//          nBase = -1;//nBase = -1 indicates output string
                pchStrTemp = (char *)*pPara;
                nStrLen = strlen(pchStrTemp);
                if (!(nOutputStyle & LEFT)) {
                    if (nOutputStyle & ZERO) {
                        while (nWidth-- > nStrLen)
                            *pchBufTemp++ = '0';
                    } else {
                        while (nWidth-- > nStrLen)
                            *pchBufTemp++ = ' ';
                    }
                }
                while (*pchStrTemp)
                    *pchBufTemp++ = *pchStrTemp++;
                while (nWidth-- > nStrLen)
                    *pchBufTemp++ = ' ';
                pPara++;
                pchFormat++;
                continue;
            case 'c' :
//          nBase = -2;//nBase = -2 indicates output charactor
                if (!(nOutputStyle & LEFT)) {
                    if (nOutputStyle & ZERO) {
                        while (nWidth-- > 1)
                            *pchBufTemp++ = '0';
                    } else {
                        while (nWidth-- > 1)
                            *pchBufTemp++ = ' ';
                    }
                }
                *pchBufTemp++ = (char) * pPara;
                while (nWidth-- > 1)
                    *pchBufTemp++ = ' ';
                pPara++;
                pchFormat++;
                continue;
            default  :
                *pchBufTemp++ = *pchFmtRoll++;
                pchFormat = pchFmtRoll;//roll back pchFormat
                continue;
        }
        pchBufTemp = NumToAscii(pchBufTemp, *pPara, nWidth, nBase, nOutputStyle);
        pPara++;
        pchFormat++;
    }
    *pchBufTemp = '\0';

    return pchBufTemp - pchBuf;
}


int sprintk(char *pBuf, const char *pchFormat, ...)
{
    int *nPara = (int *)&pchFormat + 1;
    int nPrinted;

    nPrinted = vsprintk(pBuf, pchFormat, nPara);

    return nPrinted;
}


// fixme:
// 1. add %p support
// 2. add error msg when format not supported yet
// 3. enable GCC format checking

int myprintk(const char *pchFmt, ...)
{
    int *nPara = (int *)&pchFmt + 1;
    long long tick;
    int nPrtedlen;
    int i;
    char chArraybuf[512];
    char chtime[14]; //format is [12345678.123]
    char *pchBuf = chArraybuf;

    tick = sched_get_current();
    tick = tick / 24000;
    chtime[0]  = '<';
    chtime[9]  = '.';
    chtime[13] = '>';
    for (i = 12; i > 0; i--) {
        if (9 == i) {
            continue;
        }
        chtime[i] = (char)('0' + (tick % 10));
        tick /= 10;
    }
    for (i = 0; i < 14 ; i++) {
        putchar(chtime[i]);
    }
    nPrtedlen = vsprintk(chArraybuf, pchFmt, nPara);

    while (*pchBuf) {
        putchar(*pchBuf++);
    }
    return nPrtedlen;
}

extern void LCD_ShowChar ( u16 x, u16 y, u16 fc, u16 bc, u8 num, u8 size, u8 mode );

static const int dotlocaction_9[] = {9, 7, 6, 5, 4, 3, 2, 1};
static const int dotlocaction[] = { 7, 5, 4, 3, 2, 1};
/**we use the fun dprintf_om(..),when Arraybufstart = 0,the function is exactly
   the same as dprintf_o,so we shield the  dprintf_o function**/
#if 0
u8 dprintf_o(int val, u8 dot, u16 sx, u16 sy, u8 size, u8 mode)
{
#define DISPLAY_WIDTH 7
    int  i;
    int  j;
    u8 mk = 0;
    char negative;
    char chArraybuf[DISPLAY_WIDTH + 1]; //format is [4561230.]
    static  char old64_chArraybuf[DISPLAY_WIDTH + 1]; //format is [4561230.]
    if (val >= 0) {
        i = val;
        if (i > 9999999) {
            for (j = 0; j < DISPLAY_WIDTH; j++)chArraybuf[j] = 'H';
            dot = 0; //超出范围后强制小数点为0,避免显示格式和有小数点的时候有差异
            goto fast_display;
        }
        negative = 0;
    } else {
        i = -val;
        if (i > 999999) {
            for (j = 0; j < DISPLAY_WIDTH; j++)chArraybuf[j] = 'L';
            dot = 0;
            goto fast_display;
        }
        negative = 1;
    }
    if (dot > (DISPLAY_WIDTH - 2))dot = DISPLAY_WIDTH - 2;
    for (j = DISPLAY_WIDTH - 1; j >= 0; j--) {
        chArraybuf[j] = i % 10 + '0';
        if (dotlocaction[dot] == j) {
            chArraybuf[j - 1] = chArraybuf[j];
            chArraybuf[j--] = '.';
        }
        i /= 10;
        if (!i) {
            if (--j < 0)break;
            while (j >= 0) {
                if (dotlocaction[dot] < j)   chArraybuf[j--] = '0';
                else if (dotlocaction[dot] == j) chArraybuf[j--] = '.';
                else if (dotlocaction[dot] == j + 1) chArraybuf[j--] = '0';
                else  chArraybuf[j--] = ' ';
            }
            if (negative) {
                //chArraybuf[j--] = '-';
                j = 0; //解决‘.’和‘-’切换的BUG
                while (chArraybuf[j] == ' ') {
                    j++;
                }
                chArraybuf[j - 1] = '-';
            }
            break;
        }
    }
fast_display:
    if (mode == showleft) {
        j = 0;
        while (chArraybuf[j] == ' ') {
            mk++;
            j++;
        }
    } else {
        mk = 0;
    }
    for (j = mk; j < DISPLAY_WIDTH; j++) {
        if (size == 8) {
            if (Curve.gui_paint_mode == 0) {
                LCD_ShowChar (sx + ((j - mk) * (6)), sy, RED, WHITE, chArraybuf[j], size, 0);
            } else {
                LCD_Gram_ShowChar(sx + ((j - mk) * (6)), sy, RED, WHITE, chArraybuf[j], size, 0);
            }
        } else if (size <= 40) {
            if (Curve.gui_paint_mode == 0) {
                if ((DISPLAY_WIDTH - j) > dot) {
                    LCD_ShowChar (sx + ((j - mk) * (size / 2)), sy, RED, WHITE, chArraybuf[j], size, 0);
                } else {
                    LCD_ShowChar (sx + ((j - mk) * (size / 2) - (size / 16 * 4)), sy, RED, WHITE, chArraybuf[j], size, 0);
                }
            } else {
                LCD_Gram_ShowChar(sx + ((j - mk) * (size / 2)), sy, RED, WHITE, chArraybuf[j], size, 0);
            }
        } else if (size >= 48) {
//            if ((old64_chArraybuf[j] != chArraybuf[j]) || dispaly_pwr_state) {//TFT才需要处理，上位机切换有bug
            if (chArraybuf[j] == '-') {
                chArraybuf[j] = ';'; //相当于-号
            } else if (chArraybuf[j] == '.') {
                chArraybuf[j] = '<'; //相当于.号
            } else if (chArraybuf[j] == 'H') {
                chArraybuf[j] = '='; //相当于等于号
            } else if (chArraybuf[j] == 'L') {
                chArraybuf[j] = '>'; //相当于大于号
            } else if (((chArraybuf[j] < '0') || (chArraybuf[j] > '9')) && (chArraybuf[j] != '-')) {
                chArraybuf[j] = ':'; //相当于空格
            }
            if ((DISPLAY_WIDTH - j) > dot) {
                LCD_ShowChar (sx + ((j - mk) * size / 2), sy, WHITE, BLACK, chArraybuf[j], size, 0); //sy=88,正中间显示数字
            } else {
                LCD_ShowChar (sx + ((j - mk) * (size / 2) - (size / 16 * 4)), sy, RED, WHITE, chArraybuf[j], size, 0);
            }
//            }
            old64_chArraybuf[j] = chArraybuf[j];
            dispaly_pwr_state ++ ;
        }
    }
    if (dispaly_pwr_state > 1) {
        dispaly_pwr_state = 0;
    }
    if (mode == showleft) {
        for (j = mk; j > 0; j--) {
            if (size == 8) {
                if (Curve.gui_paint_mode == 0) {
                    LCD_ShowChar (sx + ((DISPLAY_WIDTH - j) * 6), sy, RED, WHITE, ' ', size, 0);
                } else {
                    LCD_Gram_ShowChar(sx + ((DISPLAY_WIDTH - j) * 6), sy, RED, WHITE, ' ', size, 0);
                }
            } else {
                if (Curve.gui_paint_mode == 0) {
                    LCD_ShowChar (sx + ((DISPLAY_WIDTH - j) * (size / 2)), sy, RED, WHITE, ' ', size, 0);
                } else {
                    LCD_Gram_ShowChar(sx + ((DISPLAY_WIDTH - j) * (size / 2)), sy, RED, WHITE, ' ', size, 0);
                }
            }
        }
    }
    return (DISPLAY_WIDTH - mk);
}
#endif
/**Arraybufstart用法注意：当可用显示宽度小于DISPLAY_WIDTH，可以使用Arraybufstart做偏移对比，
但是需要确保val值的实际需要显示尺寸必须小于（DISPLAY_WIDTH -Arraybufstart），否则使用偏移
会导致丢失最高位，比如val = -123.56,Arraybufstart = 1,最终显示会是val = -23.56,但是如果
Arraybufstart = 0，真正显示屏又无法显示7个字，因此需要手动让val/10使低位被忽略掉**/
u8 dprintf_om(int val, u8 dot, u16 sx, u16 sy, u8 Arraybufstart, u8 size, u8 mode)
{
#define DISPLAY_WIDTH 7
    int  i;
    int  j;
    u8 mk = 0;
    char negative;
    char chArraybuf[DISPLAY_WIDTH + 1]; //format is [4561230.]
    static  char old64_chArraybuf[DISPLAY_WIDTH + 1]; //format is [4561230.]
    if (val >= 0) {
        i = val;
        if (i > 9999999) {
            for (j = 0; j < DISPLAY_WIDTH; j++)chArraybuf[j] = 'H';
            dot = 0; //超出范围后强制小数点为0,避免显示格式和有小数点的时候有差异
            goto fast_display;
        }
        negative = 0;
    } else {
        i = -val;
        if (i > 999999) {
            for (j = 0; j < DISPLAY_WIDTH; j++)chArraybuf[j] = 'L';
            dot = 0;
            goto fast_display;
        }
        negative = 1;
    }
    if (dot > (DISPLAY_WIDTH - 2))dot = DISPLAY_WIDTH - 2;
    for (j = DISPLAY_WIDTH - 1; j >= 0; j--) {
        chArraybuf[j] = i % 10 + '0';
        if (dotlocaction[dot] == j) {
            chArraybuf[j - 1] = chArraybuf[j];
            chArraybuf[j--] = '.';
        }
        i /= 10;
        if (!i) {
            if (--j < 0)break;
            while (j >= 0) {
                if (dotlocaction[dot] < j)   chArraybuf[j--] = '0';
                else if (dotlocaction[dot] == j) chArraybuf[j--] = '.';
                else if (dotlocaction[dot] == j + 1) chArraybuf[j--] = '0';
                else  chArraybuf[j--] = ' ';
            }
            if (negative) {
                //chArraybuf[j--] = '-';
                j = 0; //解决‘.’和‘-’切换的BUG
                while (chArraybuf[j] == ' ') {
                    j++;
                }
                chArraybuf[j - 1] = '-';
            }
            break;
        }
    }
fast_display:
    if (mode == showleft) {
        j = 0;
        while (chArraybuf[j] == ' ') {
            mk++;
            j++;
        }
    } else {
        mk = Arraybufstart; //右对齐的时候，人工调整从哪个字开始写，解决显示小于7位时候很大问题
    }
    for (j = mk; j < DISPLAY_WIDTH; j++) {
        if (size == 8) {
            if (Curve.gui_paint_mode == 0) {
                LCD_ShowChar (sx + ((j - mk) * (6)), sy, RED, WHITE, chArraybuf[j], size, 0);
            } else {
                LCD_Gram_ShowChar(sx + ((j - mk) * (6)), sy, RED, WHITE, chArraybuf[j], size, 0);
            }
        } else if (size <= 40) {
            if (Curve.gui_paint_mode == 0) {
                if ((DISPLAY_WIDTH - j) > dot) {
                    LCD_ShowChar (sx + ((j - mk) * (size / 2)), sy, RED, WHITE, chArraybuf[j], size, 0);
                } else {
                    LCD_ShowChar (sx + ((j - mk) * (size / 2) - (size / 16 * 4)), sy, RED, WHITE, chArraybuf[j], size, 0);
                }
            } else {
                LCD_Gram_ShowChar(sx + ((j - mk) * (size / 2)), sy, RED, WHITE, chArraybuf[j], size, 0);
            }
        } else if (size >= 48) {
//            if ((old64_chArraybuf[j] != chArraybuf[j]) || dispaly_pwr_state) {//TFT才需要处理，上位机切换有bug
            if (chArraybuf[j] == '-') {
                chArraybuf[j] = ';'; //相当于-号
            } else if (chArraybuf[j] == '.') {
                chArraybuf[j] = '<'; //相当于.号
            } else if (chArraybuf[j] == 'H') {
                chArraybuf[j] = '='; //相当于等于号
            } else if (chArraybuf[j] == 'L') {
                chArraybuf[j] = '>'; //相当于大于号
            } else if (((chArraybuf[j] < '0') || (chArraybuf[j] > '9')) && (chArraybuf[j] != '-')) {
                chArraybuf[j] = ':'; //相当于空格
            }
            if ((DISPLAY_WIDTH - j) > dot) {
                LCD_ShowChar (sx + ((j - mk) * size / 2), sy, WHITE, BLACK, chArraybuf[j], size, 0); //sy=88,正中间显示数字
            } else {
                LCD_ShowChar (sx + ((j - mk) * (size / 2) - (size / 16 * 4)), sy, RED, WHITE, chArraybuf[j], size, 0);
            }
//            }
            old64_chArraybuf[j] = chArraybuf[j];
            dispaly_pwr_state ++ ;
        }
    }
    if (dispaly_pwr_state > 1) {
        dispaly_pwr_state = 0;
    }
    if (mode == showleft) {
        for (j = mk; j > 0; j--) {
            if (size == 8) {
                if (Curve.gui_paint_mode == 0) {
                    LCD_ShowChar (sx + ((DISPLAY_WIDTH - j) * 6), sy, RED, WHITE, ' ', size, 0);
                } else {
                    LCD_Gram_ShowChar(sx + ((DISPLAY_WIDTH - j) * 6), sy, RED, WHITE, ' ', size, 0);
                }
            } else {
                if (Curve.gui_paint_mode == 0) {
                    LCD_ShowChar (sx + ((DISPLAY_WIDTH - j) * (size / 2)), sy, RED, WHITE, ' ', size, 0);
                } else {
                    LCD_Gram_ShowChar(sx + ((DISPLAY_WIDTH - j) * (size / 2)), sy, RED, WHITE, ' ', size, 0);
                }
            }
        }
    }
    return (DISPLAY_WIDTH - mk);
}
u8 dprintf_9(int val, u8 dot, u16 sx, u16 sy, u8 size, u8 mode)
{
#define DISPLAY_WIDTH_9 9
    int  i;
    int  j;
    u8 mk = 0;
    char negative;
    char chArraybuf[DISPLAY_WIDTH_9 + 1]; //format is [4561230.]
    static  char old64_chArraybuf[DISPLAY_WIDTH_9 + 1]; //format is [4561230.]
    if (val >= 0) {
        i = val;
        if (i > 99999999) {
            for (j = 0; j < DISPLAY_WIDTH_9; j++)chArraybuf[j] = 'H';
            goto fast_display;
        }
        negative = 0;
    } else {
        i = -val;
        if (i > 9999999) {
            for (j = 0; j < DISPLAY_WIDTH_9; j++)chArraybuf[j] = 'L';
            goto fast_display;
        }
        negative = 1;
    }
    if (dot > (DISPLAY_WIDTH_9 - 2))dot = DISPLAY_WIDTH_9 - 2;
    for (j = DISPLAY_WIDTH_9 - 1; j >= 0; j--) {
        chArraybuf[j] = i % 10 + '0';
        if (dotlocaction_9[dot] == j) {
            chArraybuf[j - 1] = chArraybuf[j];
            chArraybuf[j--] = '.';
        }
        i /= 10;
        if (!i) {
            if (--j < 0)break;
            while (j >= 0) {
                if (dotlocaction_9[dot] < j)   chArraybuf[j--] = '0';
                else if (dotlocaction_9[dot] == j) chArraybuf[j--] = '.';
                else if (dotlocaction_9[dot] == j + 1) chArraybuf[j--] = '0';
                else  chArraybuf[j--] = ' ';
            }
            if (negative) {
                //chArraybuf[j--] = '-';
                j = 0; //解决‘.’和‘-’切换的BUG
                while (chArraybuf[j] == ' ') {
                    j++;
                }
                chArraybuf[j - 1] = '-';
            }
            break;
        }
    }
fast_display:
    if (mode == showleft) {
        j = 0;
        while (chArraybuf[j] == ' ') {
            mk++;
            j++;
        }
    } else {
        mk = 0;
    }
    for (j = mk; j < DISPLAY_WIDTH_9; j++) {
        if (size == 8) {
            if (Curve.gui_paint_mode == 0) {
                LCD_ShowChar (sx + ((j - mk) * (6)), sy, RED, WHITE, chArraybuf[j], size, 0);
            } else {
                LCD_Gram_ShowChar(sx + ((j - mk) * (6)), sy, RED, WHITE, chArraybuf[j], size, 0);
            }
        } else if (size <= 40) {
            if (Curve.gui_paint_mode == 0) {
                if ((DISPLAY_WIDTH_9 - j) > dot) {
                    LCD_ShowChar (sx + ((j - mk) * (size / 2)), sy, RED, WHITE, chArraybuf[j], size, 0);
                } else {
                    LCD_ShowChar (sx + ((j - mk) * (size / 2) - (size / 16 * 4)), sy, RED, WHITE, chArraybuf[j], size, 0);
                }
            } else {
                LCD_Gram_ShowChar(sx + ((j - mk) * (size / 2)), sy, RED, WHITE, chArraybuf[j], size, 0);
            }
        } else if (size >= 48) {
            if ((old64_chArraybuf[j] != chArraybuf[j]) || dispaly_pwr_state) {
                if (chArraybuf[j] == '-') {
                    chArraybuf[j] = ';'; //相当于-号
                } else if (chArraybuf[j] == '.') {
                    chArraybuf[j] = '<'; //相当于.号
                } else if (chArraybuf[j] == 'H') {
                    chArraybuf[j] = '='; //相当于等于号
                } else if (chArraybuf[j] == 'L') {
                    chArraybuf[j] = '>'; //相当于大于号
                } else if (((chArraybuf[j] < '0') || (chArraybuf[j] > '9')) && (chArraybuf[j] != '-')) {
                    chArraybuf[j] = ':'; //相当于空格
                }
                if ((DISPLAY_WIDTH - j) > dot) {
                    LCD_ShowChar (sx + ((j - mk) * size / 2), sy, WHITE, BLACK, chArraybuf[j], size, 0); //sy=88,正中间显示数字
                } else {
                    LCD_ShowChar (sx + ((j - mk) * (size / 2) - (size / 16 * 4)), sy, RED, WHITE, chArraybuf[j], size, 0);
                }
            }
            old64_chArraybuf[j] = chArraybuf[j];
            dispaly_pwr_state ++ ;
        }
    }
    if (dispaly_pwr_state > 1) {
        dispaly_pwr_state = 0;
    }
    if (mode == showleft) {
        for (j = mk; j > 0; j--) {
            if (size == 8) {
                if (Curve.gui_paint_mode == 0) {
                    LCD_ShowChar (sx + ((DISPLAY_WIDTH_9 - j) * 6), sy, RED, WHITE, ' ', size, 0);
                } else {
                    LCD_Gram_ShowChar(sx + ((DISPLAY_WIDTH_9 - j) * 6), sy, RED, WHITE, ' ', size, 0);
                }
            } else {
                if (Curve.gui_paint_mode == 0) {
                    LCD_ShowChar (sx + ((DISPLAY_WIDTH_9 - j) * (size / 2)), sy, RED, WHITE, ' ', size, 0);
                } else {
                    LCD_Gram_ShowChar(sx + ((DISPLAY_WIDTH_9 - j) * (size / 2)), sy, RED, WHITE, ' ', size, 0);
                }
            }
        }
    }
    return (DISPLAY_WIDTH_9 - mk);
}
#define IF_UNOVER(InputChar) { cTemp = InputChar; if (nStrCount < nSize - 1){*pchBufTemp++ = cTemp;} ++nStrCount; }

static int vsnprintk(char *pchBuf, int nSize, const char *pchFormat, int *pPara)
{
    int nOutputStyle, nWidth, nBase, nTrueWidth, i;
    const char *pchFmtRoll;
    char *pchStrTemp;
    int nStrLen;
    char chTempArray[32];
    int nCount = 0, nStrCount = 0;
    char *pchBufTemp = pchBuf;
    char cTemp;
    char pchNum[32] = {0};
    char* pchTempNum = NULL;


    while (*pchFormat) {
        if ('%' != *pchFormat) {
            IF_UNOVER(*pchFormat++)
            continue;
        }
        pchFmtRoll = pchFormat++;

        if ('%' == *pchFormat) { //handle e.g "%%"
            IF_UNOVER(*pchFormat++)
            continue;
        }
        pchFormat--;

        nOutputStyle = 0;
    get_sign :
        pchFormat++;//skip '%', handle next charactor, and record current position
        switch (*pchFormat) {
            case '-' :
                nOutputStyle |= LEFT;
                nOutputStyle &= (~ZERO);
                goto get_sign;
            case '+' :
                nOutputStyle |= SIGN;
                goto get_sign;
            case ' ' :
                nOutputStyle |= SPACE;
                goto get_sign;
            case '#' :
                nOutputStyle |= PLUS;
                goto get_sign;
            case '0' :
                if (!(nOutputStyle & LEFT))
                    nOutputStyle |= ZERO;
                goto get_sign;
        }

        nWidth = 0;

        while (ISDIGIT(*pchFormat)) {
            nWidth = nWidth * 10 + *pchFormat - '0';
            pchFormat++;
        }

        //handle '*',replace '*'
        if ('*' == *pchFormat) {
            pchFormat++;

            if (!ISDIGIT(*pchFormat)) {
                nOutputStyle &= ~LEFT;
                nWidth = *pPara++;
            } else {
                while (*pPara) {
                    chTempArray[nCount++] = *pPara % 10 + '0';
                    *pPara /= 10;
                }

                while (nCount)
                    IF_UNOVER(chTempArray[--nCount])

                    pPara++;
                continue;
            }
        }

        nBase = 10;

        //fixme:handle '%l(o,x,i,d)' and '%ll(o,x,i,d)',filter 'l' and 'll'
        if (('l' == *pchFormat) && ('l' == *++pchFormat))
            *pchFormat ++;

        switch (*pchFormat) {
            case 'u':
                nBase = 10;
                nOutputStyle &= (~SIGN) | (~PLUS);
            case 'd' :
            case 'i' :
                nBase = 10;
//          nOutputStyle &= (~PLUS);
                break;
            case 'p' :
                nOutputStyle |= PLUS;
            case 'X' :
            case 'x' :
                nBase = 16;
//          nOutputStyle &= (~(SPACE | SIGN));
                break;
            case 'o' :
                nBase = 8;
//          nOutputStyle &= (~(SPACE | SIGN));
                break;
            case 's' :
//          nBase = -1;//nBase = -1 indicates output string
                pchStrTemp = (char *)*pPara;
                nStrLen = strlen(pchStrTemp);
                if (!(nOutputStyle & LEFT)) {
                    if (nOutputStyle & ZERO) {
                        while (nWidth-- > nStrLen)
                            IF_UNOVER('0')
                        } else {
                        myprintk("%d,%d\n", nWidth, nStrLen);
                        while (nWidth-- > nStrLen)
                            IF_UNOVER(' ')
                        }
                }
                while (*pchStrTemp)
                    IF_UNOVER(*pchStrTemp++)
                    while (nWidth-- > nStrLen)
                        IF_UNOVER(' ')
                        pPara++;
                pchFormat++;
                continue;
            case 'c' :
//          nBase = -2;//nBase = -2 indicates output charactor
                if (!(nOutputStyle & LEFT)) {
                    if (nOutputStyle & ZERO) {
                        while (nWidth-- > 1)
                            IF_UNOVER('0');
                    } else {
                        while (nWidth-- > 1)
                            IF_UNOVER(' ');
                    }
                }
                IF_UNOVER((char)*pPara);
                while (nWidth-- > 1)
                    IF_UNOVER(' ');
                pPara++;
                pchFormat++;
                continue;
            default  :
                IF_UNOVER(*pchFmtRoll++);
                pchFormat = pchFmtRoll;//roll back pchFormat
                continue;
        }
        pchTempNum = pchNum;
        pchTempNum = NumToAscii(pchTempNum, *pPara, nWidth, nBase, nOutputStyle);
        nTrueWidth = strlen(pchNum);
        for (i = 0; i < nTrueWidth; ++i) {
            IF_UNOVER(pchNum[i])
        }

        pPara++;
        pchFormat++;
    }
    *pchBufTemp = '\0';

    return nStrCount;
}


int snprintk(char *pBuf, int nSize, const char *pchFormat, ...)
{
    int *nPara = (int *)&pchFormat + 1;
    int nPrinted;

    nPrinted = vsnprintk(pBuf, nSize, pchFormat, nPara);

    return nPrinted;
}

int fflush1(int fd)
{
    return 0;
}

void ClearScreen(void)
{
    myprintk("\033[2J");
}



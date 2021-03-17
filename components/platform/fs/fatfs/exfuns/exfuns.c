#include "string.h"
#include "exfuns.h"
#include "fattester.h"
#include "malloc.h"

#define FILE_MAX_TYPE_NUM       7   //最多FILE_MAX_TYPE_NUM个大类
#define FILE_MAX_SUBT_NUM       4   //最多FILE_MAX_SUBT_NUM个小类

//文件类型列表
const char*  FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM] = {
    {"BIN"},            //BIN文件
    {"LRC"},            //LRC文件
    {"NES"},            //NES文件
    {"TXT", "C", "H", "CSV"}, //文本文件
    {"WAV", "MP3", "APE", "FLAC"}, //支持的音乐文件
    {"BMP", "JPG", "JPEG", "GIF"}, //图片文件
    {"AVI"},            //视频文件
};


//将小写字母转为大写字母,如果是数字,则保持不变.
u8 char_upper(u8 c)
{
    if (c < 'A')return c; //数字,保持不变.
    if (c >= 'a')return c - 0x20; //变为大写.
    else return c;//大写,保持不变
}
//报告文件的类型
//fname:文件名
//返回值:0XFF,表示无法识别的文件类型编号.
//       其他,高四位表示所属大类,低四位表示所属小类.
u8 f_typetell(u8 *fname)
{
    static u8 tbuf[5];  //如果不使用静态内存会出差，后面两个数会指向其他地方//debug ling
    u8 *attr = '\0'; //后缀名
    u8 i = 0, j;
    while (i < 250) {
        i++;
        if (*fname == '\0')break; //偏移到了最后了.
        fname++;
    }
    if (i == 250)return 0XFF; //错误的字符串.
    for (i = 0; i < 5; i++) { //得到后缀名
        fname--;
        if (*fname == '.') {
            fname++;
            attr = fname;
            break;
        }
    }
    strcpy((char *)tbuf, (const char*)attr); //copy
    for (i = 0; i < 4; i++)tbuf[i] = char_upper(tbuf[i]); //全部变为大写
    for (i = 0; i < FILE_MAX_TYPE_NUM; i++) { //大类对比
        for (j = 0; j < FILE_MAX_SUBT_NUM; j++) { //子类对比
            if (*FILE_TYPE_TBL[i][j] == 0)break; //此组已经没有可对比的成员了.
            if (strcmp((const char *)FILE_TYPE_TBL[i][j], (const char *)tbuf) == 0) { //找到了
                return (i << 4) | j;
            }
        }
    }
    return 0XFF;//没找到
}












